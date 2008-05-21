#include "poor_xml/frl_poor_xml_parser.h"
#include "poor_xml/frl_poor_xml_document.h"

namespace frl
{
namespace poor_xml
{
frl::String Parser::getFirstNodeName( const String & buffer, size_t &startPos, size_t &endPos )
{			
	if( buffer.empty() )
		FRL_THROW_S_CLASS( Parser::InvalidArgument );
	size_t i = 0;
	Bool openBrackets1 = False;
	Bool openBrackets2 = False;
	String name;
	while( i < buffer.length() )
	{
		switch( buffer[i] )
		{
		case FRL_STR('<'):
			{
				if( ! openBrackets1 && ! openBrackets2 && i < buffer.length() - 1 )
				{
					++i;
					startPos = i;
					while(	i < buffer.length() 
						&& buffer[i] != FRL_STR(' ') 
						&& buffer[i] != FRL_STR('\x09')
						&& buffer[i] != FRL_STR('>') )
					{
						if( buffer[i] == FRL_STR('/') )
						{
							if( i < buffer.length() - 1 )
							{
								if( buffer[i+1] == FRL_STR('>') )
								{
									if( name.empty() )
										FRL_THROW_S_CLASS( Parser::BadNodeName );
									return name;
								}
								else
									FRL_THROW_S_CLASS( Parser::BadNodeName );
							}
							else
								FRL_THROW_S_CLASS( Parser::BadNodeName );
						}
						name += buffer[i];
						++i;
					}
					endPos = i;
					return name;
				}
			}
			break;
		case FRL_STR('\''):
			if( openBrackets1 )
				openBrackets1 = False;
			else
				openBrackets1 = True;
			break;

		case FRL_STR('\"'):
			if( openBrackets2 )
				openBrackets2 = False;
			else
				openBrackets2 = True;
			break;

		}
		++i;
	}
	if( name.empty() )
		FRL_THROW_S_CLASS( Parser::BadNodeName );
	return name;
}

size_t Parser::findNodeClose( const String &buffer, const String &nodeName, Bool &simple )
{
	String tmpNodeName = FRL_STR("<") + nodeName;
	size_t tmpPos = buffer.find( tmpNodeName );
	if( tmpPos == String::npos )
		FRL_THROW_S_CLASS( Parser::InvalidArgument );

	size_t pos = buffer.find( FRL_STR("</") + nodeName + FRL_STR(">") );
	if( pos != String::npos )
	{
		simple = False;
		return pos;
	}

	size_t i = tmpPos;
	Bool openBrackets1 = False;
	Bool openBrackets2 = False;
	pos = String::npos;
	while( i < buffer.length() )
	{
		if( buffer[i] == FRL_STR('\'') )
		{
			if( openBrackets1 )
				openBrackets1 = False;
			else
				openBrackets1 = True;
		}

		if( buffer[i] == FRL_STR('\"') )
		{
			if( openBrackets2 )
				openBrackets2 = False;
			else
				openBrackets2 = True;
		}

		if( !openBrackets1 && ! openBrackets2 )
		{
			if( buffer[i] == FRL_STR('/') && i < buffer.length() - 1 )
			{
				if( buffer[ i+1 ] == FRL_STR('>') )	
				{
					pos = i;
					simple = True;
					break;
				}
			}
		}
		++i;
	}
	if( pos == String::npos )
		FRL_THROW_S_CLASS( Document::BrokenXML );
	return pos;
}

size_t Parser::findCloseBaracket( const String &buffer, size_t fromPos )
{
	if( fromPos >= buffer.length() )
		FRL_THROW_S_CLASS( Parser::InvalidArgument );

	size_t i = fromPos;
	Bool openBrackets1 = False;
	Bool openBrackets2 = False;
	size_t pos = String::npos;
	while( i < buffer.length() )
	{
		if( buffer[i] == FRL_STR('\'') )
		{
			if( openBrackets1 )
				openBrackets1 = False;
			else
				openBrackets1 = True;
		}

		if( buffer[i] == FRL_STR('\"') )
		{
			if( openBrackets2 )
				openBrackets2 = False;
			else
				openBrackets2 = True;
		}

		if( !openBrackets1 && ! openBrackets2 )
		{
			if( buffer[i] == FRL_STR('>') )
			{
				pos = i;
				break;
			}
		}
		++i;
	}
	if( pos == String::npos )
		FRL_THROW_S_CLASS( Document::BrokenXML );
	return pos;
}

NodesList Parser::getSubNodes( const String &buffer )
{
	NodesList ret;
	if( buffer.empty() )
		return ret;

	String tmpBuffer = buffer;
	String tmpBuffer2;
	for( ; ; )
	{
		if( tmpBuffer.empty() )
			break;

		size_t startNamePos = 0;
		size_t endNamePos = 0;
		String nodeName;
		try
		{
			nodeName = getFirstNodeName( tmpBuffer, startNamePos, endNamePos );
		}
		catch( Parser::BadNodeName& )
		{
			break;
		}
		Bool simple;
		size_t closePos = findNodeClose( tmpBuffer, nodeName, simple );
		size_t endNodeDef = closePos;
		String propetriesStr;
		if( simple )
		{
			propetriesStr = tmpBuffer.substr( endNamePos, closePos -endNamePos );
		}
		else
		{
			endNodeDef = findCloseBaracket( tmpBuffer, endNamePos );
			propetriesStr = tmpBuffer.substr( endNamePos, endNodeDef - endNamePos );
		}

		SmartPtr< Node > newNode( new Node( nodeName ) );
		ret.push_back( newNode );
		newNode->properties = getProperties( propetriesStr );
		if( ! simple )
		{	
			tmpBuffer2 = tmpBuffer.substr( endNodeDef + 1, closePos - endNodeDef - 1 );
			removeSimbolsFromStart( tmpBuffer2, FRL_STR(' ') );
			removeSimbolsFromEnd( tmpBuffer2, FRL_STR(' ') );
			removeSimbolsFromStart( tmpBuffer2, FRL_STR('\x09') );
			removeSimbolsFromEnd( tmpBuffer2, FRL_STR('\x09') );
			newNode->nodes = getSubNodes( tmpBuffer2 );
		}
		if( ( closePos + nodeName.length() + 3 ) > tmpBuffer.length() )
			break;

		if( ! simple )
			tmpBuffer = tmpBuffer.substr( closePos + nodeName.length() + 3, tmpBuffer.length() - 1  );
		else
			tmpBuffer = tmpBuffer.substr( closePos + 2, tmpBuffer.length() - 1  );

		removeSimbolsFromStart( tmpBuffer, FRL_STR(' ') );
		removeSimbolsFromEnd( tmpBuffer, FRL_STR(' ') );
		removeSimbolsFromStart( tmpBuffer, FRL_STR('\x09') );
		removeSimbolsFromEnd( tmpBuffer, FRL_STR('\x09') );
	}
	return ret;
}

std::map< String, String > Parser::getProperties( const String &buffer )
{
	std::map< String, String > properties;
	if( buffer.empty() )
		return properties;
	String tmpBuffer = buffer;
	for( ; ; )
	{
		size_t equalSimbol = tmpBuffer.find( FRL_STR("=") );
		if( equalSimbol == String::npos )
			break;
		if( equalSimbol == 0 || equalSimbol == tmpBuffer.length() - 3 ) // ="">
			FRL_THROW_S_CLASS( Document::BrokenXML );
		size_t spaceSimbol = tmpBuffer.rfind( FRL_STR(" "), equalSimbol );
		size_t tabSimbol = tmpBuffer.rfind( FRL_STR('\x09'), equalSimbol );
		if( spaceSimbol == String::npos && tabSimbol == String::npos )
			FRL_THROW_S_CLASS( Document::BrokenXML );
		if( tabSimbol < spaceSimbol )
			spaceSimbol = tabSimbol;
		if( spaceSimbol == String::npos )
			FRL_THROW_S_CLASS( Document::BrokenXML );
		if( equalSimbol <= spaceSimbol )
			FRL_THROW_S_CLASS( Document::BrokenXML );
		String propertyName = tmpBuffer.substr( spaceSimbol, equalSimbol - spaceSimbol );
		removeSimbolsFromStart( propertyName, FRL_STR(' ') );
		removeSimbolsFromStart( propertyName, FRL_STR('\x09') );
		removeSimbolsFromEnd( propertyName, FRL_STR(' ') );
		removeSimbolsFromEnd( propertyName, FRL_STR('\x09') );

		size_t bracketsOneOpen = tmpBuffer.find( FRL_STR("\'"), equalSimbol );
		size_t bracketsTwoOpen = tmpBuffer.find( FRL_STR("\""), equalSimbol );

		if( bracketsOneOpen == String::npos && bracketsTwoOpen == String::npos )
			FRL_THROW_S_CLASS( Document::BrokenXML );

		size_t bracketsClose;
		if( bracketsTwoOpen < bracketsOneOpen )
		{
			bracketsClose = tmpBuffer.find( FRL_STR("\""), bracketsTwoOpen + 1 );
			bracketsOneOpen = bracketsTwoOpen;
		}
		else
			bracketsClose = tmpBuffer.find( FRL_STR('\"'), bracketsTwoOpen + 1 );

		if( bracketsClose == String::npos )
			FRL_THROW_S_CLASS( Document::BrokenXML );

		String propertyVal = tmpBuffer.substr( bracketsOneOpen+1, bracketsClose - bracketsOneOpen - 1 );

		properties.insert( std::pair< String, String >( propertyName, propertyVal) );

		tmpBuffer = tmpBuffer.substr( bracketsClose+1, tmpBuffer.length() - 1  );
	}
	return properties;
}

String Parser::getProperty( const String &buffer, const String &propertyName )
{
	String tmpPropName = propertyName + FRL_STR( "=" );
	size_t beginPos = buffer.find( tmpPropName );
	if( beginPos == String::npos )
		FRL_THROW_S_CLASS( Parser::PropertyNotFound );
	size_t endPos = beginPos + tmpPropName.length();
	if( endPos >= buffer.length() - 2 )
		FRL_THROW_S_CLASS( Parser::InvalidProperty );
	if( ( buffer[ endPos] != FRL_STR('\'') ) && ( buffer[ endPos] != FRL_STR('\"') ) )
		FRL_THROW_S_CLASS( Parser::InvalidProperty );
	size_t tmp;
	if( buffer[ endPos ] == FRL_STR('\"') )
		tmp = buffer.find( FRL_STR("\""), endPos+1 );
	else
		tmp = buffer.find( FRL_STR("\'"), endPos+1 );
	if( tmp == String::npos )
		FRL_THROW_S_CLASS( Parser::InvalidProperty );
	String ret = buffer.substr( endPos + 1, tmp - endPos - 1 );
	return ret; // NRVO
}
} // namespace poor_xml
} // FatRat Library
