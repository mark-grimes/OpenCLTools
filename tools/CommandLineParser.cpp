#include "CommandLineParser.h"

#include <stdexcept>
#include <getopt.h>


void tools::CommandLineParser::addOption( const std::string& name, ArgumentType argumentType )
{
	allowedOptions_.push_back( std::make_pair( name, argumentType ) );
}

void tools::CommandLineParser::parse( const int argc, char* argv[] )
{
	// Don't know why this method would be called more than once, but I might as well
	// make sure I'm starting with a clean slate.
	parsedOptions_.clear();
	nonOptionArguments_.clear();

	// Suppress the getopt default error because I'll throw an exception if something
	// goes wrong.
	opterr=0;

	// First figure out what the executable name is. If there is any path prepended
	// to it strip that off.
	executableName_=argv[0];
	size_t lastSlashPosition=executableName_.find_last_of('/');
	if( lastSlashPosition!=std::string::npos ) executableName_=executableName_.substr( lastSlashPosition+1, std::string::npos );

	// First need to turn the vector allowedOptions_ into something that getopt_long
	// can understand.
	struct option longOptions[allowedOptions_.size()+1];
	for( size_t index=0; index<allowedOptions_.size(); ++index )
	{
		longOptions[index].name=allowedOptions_[index].first.c_str();
		longOptions[index].has_arg=allowedOptions_[index].second;
		longOptions[index].flag=0;
		longOptions[index].val=0;
	}
	// getopt_long instructions say the last entry has to be all zeros
	longOptions[allowedOptions_.size()].name=0;
	longOptions[allowedOptions_.size()].has_arg=0;
	longOptions[allowedOptions_.size()].flag=0;
	longOptions[allowedOptions_.size()].val=0;

	int getoptReturn;

	do
	{
		int optionIndex;
		int currentInd=optind;
		getoptReturn=getopt_long( argc, argv, "", longOptions, &optionIndex );

		if( getoptReturn==0 )
		{
			// Creating this reference will put a new entry in the map, so
			// there will be a record that an option has been specified even
			// if I don't add an argument into the vector.
			std::vector<std::string>& optionArguments=parsedOptions_[longOptions[optionIndex].name];
			if( optarg ) optionArguments.push_back( optarg );
		}
		else if( getoptReturn=='?' ) throw std::runtime_error(std::string("Unknown option, or missing argument for \"")+argv[currentInd]+"\"");
		else if( getoptReturn!=-1 )  throw std::runtime_error("getopt_long returned an unknown code");

	} while( getoptReturn!=-1 );

	// Now that all of the options have been parsed, make a note of any other command line
	// arguments.
	while( optind<argc ) nonOptionArguments_.push_back( argv[optind++] );
}

bool tools::CommandLineParser::optionHasBeenSet( const std::string& optionName ) const
{
	// Search the map to see if anything with the supplied name exists
	const auto& findResult=parsedOptions_.find(optionName);

	// Return true if the name is in the map
	return findResult!=parsedOptions_.end();
}

const std::vector<std::string>& tools::CommandLineParser::optionArguments( const std::string& optionName ) const
{
	// Search the map to see if anything with the supplied name exists
	const auto& iFindResult=parsedOptions_.find(optionName);

	if( iFindResult==parsedOptions_.end() ) return emptyVector_;
	else return iFindResult->second;
}

const std::vector<std::string>& tools::CommandLineParser::nonOptionArguments() const
{
	return nonOptionArguments_;
}

const std::string& tools::CommandLineParser::executableName() const
{
	return executableName_;
}
