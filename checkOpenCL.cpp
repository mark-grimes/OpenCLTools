/** @file
 *
 * Simple host only OpenCL program that queries the platforms and devices available
 * and dumps some information to stdout.
 *
 * Compile with:
 *     clang++ --std=c++11 --stdlib=libc++ -I$HOME/Programs/OpenCL/AMDAPPSDK-3.0/include -L$HOME/Programs/OpenCL/AMDAPPSDK-3.0/lib/x86_64/sdk -l OpenCL checkOpenCL.cpp tools/CommandLineParser.cpp -o checkOpenCL -Wno-deprecated-declarations -ggdb
 */
//#define __CL_ENABLE_EXCEPTIONS
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <CL/cl.hpp>
#include "tools/CommandLineParser.h"
#include "tools/OpenCLEnums.h"

const char *TestKernel="\n" \
"__kernel void square( __global float* input, const unsigned long inputCount, __global float* output, const unsigned long outputCount ) \n" \
"{                                                                                               \n" \
"   int index=get_global_id(0);                                                                  \n" \
"   //printf( \"input is %f\\n\", input[index] );                                                  \n" \
"   if( index<inputCount && index<outputCount ) output[index] = input[index]*input[index];       \n" \
"}                                                                                               \n";

std::vector<cl::Device> getAllDevices()
{
	std::vector<cl::Device> allDevices; // return value

	std::vector<cl::Platform> platformList;
	cl::Platform::get (&platformList);
	for( const auto& platform : platformList )
	{
		std::vector<cl::Device> deviceList;
		platform.getDevices( CL_DEVICE_TYPE_ALL, &deviceList );
		allDevices.insert( allDevices.end(), deviceList.begin(), deviceList.end() );
	}

	return allDevices;
}

/** @brief Call device.getInfo<CL_DEVICE_PLATFORM>() and remove leading spaces.
 *
 * Intel platform puts spaces at the start of the device name. Small issue, but
 * for some reason it really annoys me when I see it printed out.*/
std::string formattedDeviceName( const cl::Device& device )
{
	std::string deviceName = device.getInfo<CL_DEVICE_NAME>();
	return deviceName.substr( deviceName.find_first_not_of(' ') ); // strip off leading spaces
}

std::string deviceInformationString( const cl::Device& device, bool includePlatform=true )
{
	std::string returnValue="'" + formattedDeviceName(device) + "'" + " - " + tools::deviceType( device.getInfo<CL_DEVICE_TYPE>() );
	if( includePlatform )
	{
		cl::Platform platform( device.getInfo<CL_DEVICE_PLATFORM>() );
		returnValue+=" - '" + platform.getInfo<CL_PLATFORM_NAME>() + "'";
	}
	return returnValue;
}

void printDevices( const std::vector<cl::Device>& devices, std::ostream& output=std::cout, bool printPlatform=true )
{
	for( size_t index=0; index<devices.size(); ++index )
	{
		output << index << ": " << deviceInformationString( devices[index], printPlatform ) << std::endl;
	}
}

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " [--print] [--execute] [--spir <filename>] [--device <number>] [--repeat <number>]" << "\n"
			<< "\t\t" << "--print     Print information on available OpenCL devices (default if no other action specified)." << "\n"
			<< "\t\t" << "--execute   Execute a test kernel on the selected device(s)." << "\n"
			<< "\t\t" << "--spir      Execute the supplied spir binary on the selected device(s)." << "\n"
			<< "\t\t" << "--device    The device to run on (integer matching output from '--print'). Can be specified multiple times. Default is all devices." << "\n"
			<< "\t\t" << "--repeat    Number of times to repeat execution (to try and check for race conditions). Negative numbers will repeat forever until ctrl-c." << "\n"
			<< "\t\t" << "--datasize  The size of the test dataset to run on. Default 4096." << "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message and exits" << "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
	bool printDeviceInfo=false;
	bool executeKernel=false;
	std::vector<std::string> executeSpirFiles;
	std::vector<size_t> devicesToUse;
	int timesToRepeat=1;
	size_t dataSize=4096;

	tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "print", tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "execute", tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "spir", tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "device", tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "repeat", tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "datasize", tools::CommandLineParser::RequiredArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName(), std::cerr );
			return 0;
		}

		if( commandLineParser.optionHasBeenSet( "print" ) ) printDeviceInfo=true;
		if( commandLineParser.optionHasBeenSet( "execute" ) ) executeKernel=true;
		if( commandLineParser.optionHasBeenSet( "spir" ) ) executeSpirFiles=commandLineParser.optionArguments("spir");
		// If none of these are set, then default to "print"
		if( !printDeviceInfo && !executeKernel && executeSpirFiles.empty() ) printDeviceInfo=true;

		if( commandLineParser.optionHasBeenSet( "device" ) )
		{
			for( const auto& argument : commandLineParser.optionArguments("device") )
			{
				try{ devicesToUse.push_back( std::stoi(argument) ); }
				catch( std::exception& error ) { std::cerr << " Error! '" << argument << "' is an invalid device number!" << std::endl; }
			}
		}

		if( commandLineParser.optionHasBeenSet( "repeat" ) )
		{
			try{ timesToRepeat=std::stoi( commandLineParser.optionArguments("repeat").back() ); }
			catch( std::exception& error ) { std::cerr << " Error! '" << commandLineParser.optionArguments("repeat").back() << "' is an invalid number of times to repeat execution!" << std::endl; }
		}

		if( commandLineParser.optionHasBeenSet( "datasize" ) )
		{
			std::string argument=commandLineParser.optionArguments("datasize").back();
			try
			{
				int newSize=std::stoi( argument );
				if( newSize<=0 ) std::cerr << " Error! '" << newSize << "' must be a non zero positive integer for --datasize" << std::endl;
				else dataSize=static_cast<size_t>(newSize);
			}
			catch( std::exception& error ) { std::cerr << " Error! '" << argument << "' must be a non zero positive integer for --datasize" << std::endl; }
		}
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception parsing command line: " << error.what() << std::endl;
		printUsage( commandLineParser.executableName(), std::cerr );
		return -1;
	}

	try
	{
		const auto& devices=getAllDevices();
		if( devices.empty() ) throw std::runtime_error( "There are no OpenCL devices available!" );

		// If no devices have been asked for, use the first one
		if( devicesToUse.empty() ) for( size_t index=0; index<devices.size(); ++index ) devicesToUse.push_back(index);

		if( printDeviceInfo ) printDevices( devices );

		//
		// See if I can open the SPIR files requested
		//
		std::vector< std::vector<char> > binaries;
		for( const auto& filename : executeSpirFiles )
		{
			std::ifstream spirFile( filename, std::ios::binary | std::ios::ate ); // Open at end to get the length
			if( !spirFile.is_open() ) std::cerr << "Unable to open SPIR file " << filename << std::endl;
			else
			{
				binaries.emplace_back( spirFile.tellg() ); // Create a new std::vector<char> the same size as the file
				spirFile.seekg( 0, std::ios::beg ); // Jump back to start
				// Copy into this char vector
				std::copy( std::istreambuf_iterator<char>(spirFile), std::istreambuf_iterator<char>(), binaries.back().begin() );
			}
		}

		typedef float T_input;
		typedef float T_output;
		std::vector<T_input> data(dataSize); // Arbitrary input data
		std::vector<T_output> results(dataSize);
		for( size_t index=0; index<dataSize; ++index ) data[index]=rand();

		if( !binaries.empty() || executeKernel )
		{
			// Note that it's intentional to repeat forever if timesToRepeat is negative (quit with ctrl-c)
			for( int repetitionIndex=0; repetitionIndex!=timesToRepeat; ++repetitionIndex )
			{
				for( const auto deviceNumber : devicesToUse )
				{
					cl_int error=CL_SUCCESS;

					if( deviceNumber>=devices.size() )
					{
						std::cerr << "Error! There is no device numbered " << deviceNumber << ". There are only " << devices.size() << " devices." << std::endl;
						continue;
					}
					const auto& device=devices[deviceNumber];
					std::cout << "Attempting to run on device " << deviceInformationString(device) << std::endl;

					cl::Context context( device, nullptr, nullptr, nullptr, &error );
					if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when creating context - "+tools::contextCreateError(error) );

					cl::CommandQueue queue( context, device );
					cl::Buffer input( context, CL_MEM_READ_ONLY, sizeof(T_input)*data.size() );
					cl::Buffer output( context, CL_MEM_WRITE_ONLY, sizeof(T_input)*data.size() );
					error=queue.enqueueWriteBuffer( input, CL_TRUE, 0, sizeof(T_input)*data.size(), data.data() );
					if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when copying input in" );

					std::vector<cl::Program> openCLPrograms;
					if( executeKernel )
					{
						cl::Program temp(context,TestKernel,false,&error);
						if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when creating program from source - "+tools::createProgramError(error) );
						openCLPrograms.push_back( std::move(temp) );
					}
					for( const auto binary : binaries )
					{
						cl::Program::Binaries clBinaries;
						clBinaries.push_back( std::make_pair( static_cast<const void*>(binary.data()), binary.size() ) );
						cl::Program temp(context,std::vector<cl::Device>(1,device),clBinaries,nullptr,&error);
						if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when creating program from binary - "+tools::createProgramError(error) );
						openCLPrograms.push_back( std::move(temp) );
					}

					for( const auto& program : openCLPrograms )
					{
						error=program.build();
						if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when building program:\n "+program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) );

						//
						// Figure out the kernel name and get it
						//
						cl::Kernel kernel;
						{ // limit scope of local variables
							std::string kernelName=program.getInfo<CL_PROGRAM_KERNEL_NAMES>(&error);
							if( error!=CL_SUCCESS ) throw std::runtime_error( "Error getting the kernel name" );
							kernel=cl::Kernel( program, kernelName.c_str(), &error );
							if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when creating kernel '"+kernelName+"' - "+tools::createKernelError(error) );
						}
						size_t local=kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);
						if( local>data.size() ) local=data.size();

						//
						// Set Kernel arguments
						//
						error=kernel.setArg( 0, input ); if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when setting kernel argument 0: "+tools::setKernelArgError(error) );
						error=kernel.setArg( 1, static_cast<unsigned long>(data.size()) ); if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when setting kernel argument 1: "+tools::setKernelArgError(error) );
						error=kernel.setArg( 2, output ); if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when setting kernel argument 2: "+tools::setKernelArgError(error) );
						error=kernel.setArg( 3, static_cast<unsigned long>(data.size()) ); if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when setting kernel argument 3: "+tools::setKernelArgError(error) );

						//
						// Run the kernel
						//
						error=queue.enqueueNDRangeKernel( kernel, 0, data.size(), local );
						if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when enqueing kernel - "+tools::enqueKernelError(error) );

						queue.finish();

						//
						// Get and check output
						//
						error=queue.enqueueReadBuffer( output, CL_TRUE, 0, sizeof(T_output)*results.size(), results.data() );
						if( error!=CL_SUCCESS ) throw std::runtime_error( "Error when copying output out" );

						size_t correctResults=0;
						for( size_t index=0; index<data.size() && index<results.size(); ++index )
						{
							if( results[index]==(data[index]*data[index]) ) ++correctResults;
						}
						std::cout << "   " << correctResults << "/" << data.size() << " correct results." << std::endl;
					} // end of loop over openCLPrograms
				} // end of loop over devicesToUse
			} // end of loop over timesToRepeat
		} // end of "if( !binaries.empty() || executeKernel )
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception while executing: " << error.what() << std::endl;
		return -2;
	}

	return 0;
}
