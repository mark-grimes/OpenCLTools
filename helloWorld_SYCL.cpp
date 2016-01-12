/* Compiled with:
 * compute++ --gcc-toolchain=/cm/shared/languages/GCC-4.8.4 -O2 --std=c++11 -sycl -intelspirmetadata -emit-llvm -D__DEVICE_SPIR32__ -DBUILD_PLATFORM_SPIR -I$HOME/Programs/ComputeCpp/include -o helloWorld_SYCL.cpp.bc -c helloWorld_SYCL.cpp && compute++ --gcc-toolchain=/cm/shared/languages/GCC-4.8.4 -O2 --std=c++11 -I$HOME/Programs/ComputeCpp/include -L$HOME/Programs/ComputeCpp/lib -lSYCL -L$HOME/Programs/OpenCL/AMDAPPSDK-3.0/lib/x86_64/sdk -lOpenCL helloWorld_SYCL.cpp --include helloWorld_SYCL.cpp.sycl -o helloWorld_SYCL ~/Programs/glibc-install/lib/only_libc/libc.so.6 -Wl,-rpath,$HOME/Programs/glibc-install/lib/only_libc -ggdb
 */
#include <CL/sycl.hpp>
#include <iostream>
#include <iomanip>

// Use a static data size for simplicity
//
#define DATA_SIZE (20)

//
// Unnamed namespace for things only used in this file
//
namespace
{
	/** @brief SYCL device selector that will choose Xeon Phis */
	class XeonPhiDeviceSelector : public cl::sycl::device_selector
	{
	public:
		virtual int operator()( const cl::sycl::device& device ) const override
		{
			// If it doesn't support the cl_khr_spir extension, can't use it at all
			if( !device.has_extension("cl_khr_spir") ) return -1; // Negative numbers mean it will never be used

			auto type=device.get_info<cl::sycl::info::device::device_type>();
			if( type==cl::sycl::info::device_type::accelerator ) return 100;
			else if( type==cl::sycl::info::device_type::gpu ) return 90;
			else if( type==cl::sycl::info::device_type::cpu )
			{
				// On my system I have the AMD OpenCL implementation that can run on Intel chips,
				// and I also have the Intel OpenCL implementation. I've heard the Intel implementation
				// is much faster (no proof of that though). It also reports larger work group sizes.
				std::string vendor=device.get_platform().get_info<cl::sycl::info::platform::vendor>();
				if( vendor.substr(0,5)=="Intel" ) return 80;
				else return 70;
			}
			else return 1;
		}
	};
} // end of the unnamed namespace

int main( int argc, char* argv[] )
{
	//
	// Example piece of code that takes an arbitrary number of 3D points and for
	// each point calculates which of the other points is closest in Cartesian space.
	// Uses brute force to calculate the distances between all points, then loops over
	// these distances to find the lowest value.
	//
    unsigned int count = DATA_SIZE;
	std::array<float,DATA_SIZE> data;
    for( size_t index=0; index<count; ++index ) data[index]=rand()/static_cast<float>(RAND_MAX);

	cl::sycl::buffer<float> input( data.data(), cl::sycl::range<1>(data.size()) );
	cl::sycl::buffer<float> output( cl::sycl::range<1>(data.size()) );

	::XeonPhiDeviceSelector deviceSelector;
	cl::sycl::queue myQueue(deviceSelector);

	myQueue.submit( [&]( cl::sycl::handler& myHandler )
	{
		auto inputAccess=input.get_access<cl::sycl::access::mode::read>(myHandler);
		auto outputAccess=output.get_access<cl::sycl::access::mode::write>(myHandler);

		myHandler.parallel_for<class HelloWorld>( cl::sycl::range<1>(input.get_count()), [=](cl::sycl::item<1> range)
		{
			float temp=inputAccess[range];
			outputAccess[range]=temp*temp;
			// Bizarelly, this next line gives incorrect results unless there is either
			// a temporary variable (as above), or a printf.
			//outputAccess[range]=inputAccess[range]*inputAccess[range];
		} );
	});


	auto outputCheck=output.get_access<cl::sycl::access::mode::read,cl::sycl::access::target::host_buffer>();
	size_t correctAnswers=0;
	for( size_t index=0; index<input.get_count(); ++index )
	{
		if( outputCheck[index]==(data[index]*data[index]) ) ++correctAnswers;
		//else std::cout << data[index] << "*" << data[index] << "!=" << outputCheck[index] << "!\n";
	}
	std::cout << "There were " << correctAnswers << "/" << input.get_count() << " correct answers." << std::endl;

}
