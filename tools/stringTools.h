namespace tools
{
	std::vector<std::string> splitByWhitespace( const std::string& stringToSplit )
	{
		const char* whitespace="\x20\x09\x0D\x0A";

		std::vector<std::string> returnValue;

		size_t currentPosition=0;
		size_t nextDelimeter=0;
		do
		{
			// Skip over any leading whitespace
			size_t nextElementStart=stringToSplit.find_first_not_of( whitespace, currentPosition );
			if( nextElementStart != std::string::npos ) currentPosition=nextElementStart;

			// Find the next whitespace and subtract everything up to that point
			nextDelimeter=stringToSplit.find_first_of( whitespace, currentPosition );
			std::string element=stringToSplit.substr( currentPosition, nextDelimeter - currentPosition );
			returnValue.push_back( element );

			// skip over any trailing whitespace
			nextElementStart=stringToSplit.find_first_not_of( whitespace, nextDelimeter );
			if( nextElementStart != std::string::npos ) currentPosition=nextElementStart;
			else nextDelimeter=std::string::npos;

		} while( nextDelimeter!=std::string::npos );

		return returnValue;
	}

} // end of namespace tools
