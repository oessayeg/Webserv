#include "ErrorString.class.hpp"

ErrorString::ErrorString( void )
{
	_errorFile = "<!DOCTYPE html>\n\
<html lang='en'>\n\
\n\
<head>\n\
	<meta charset='UTF-8'>\n\
	<meta http-equiv='X-UA-Compatible' content='IE=edge'>\n\
	<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n\
	<title>Document</title>\n\
<style>\n\
\n\
\n\
body {\n\
  display: flex;\n\
  flex-flow: row wrap;\n\
  align-content: center;\n\
  justify-content: center;\n\
}\n\
\n\
div {\n\
  width: 100%;\n\
  text-align: center;\n\
}\n\
\n\
.number {\n\
  background: #fff;\n\
  position: relative;\n\
  font: 900 30vmin 'Consolas';\n\
  letter-spacing: 5vmin;\n\
  text-shadow: 2px -1px 0 #000, 4px -2px 0 #0a0a0a, 6px -3px 0 #0f0f0f, 8px -4px 0 #141414, 10px -5px 0 #1a1a1a, 12px -6px 0 #1f1f1f, 14px -7px 0 #242424, 16px -8px 0 #292929;\n\
}\n\
.number::before {\n\
  background-color: #1C2331;\n\
  background-image: radial-gradient(closest-side at 50% 50%, #FF7F50 100%, rgba(0, 0, 0, 0)), radial-gradient(closest-side at 50% 50%, #FFD700 100%, rgba(0, 0, 0, 0));\n\
  background-repeat: repeat-x;\n\
  background-size: 40vmin 40vmin;\n\
  background-position: -100vmin 20vmin, 100vmin -25vmin;\n\
  width: 100%;\n\
  height: 100%;\n\
  mix-blend-mode: screen;\n\
  -webkit-animation: moving 10s linear infinite both;\n\
          animation: moving 10s linear infinite both;\n\
  display: block;\n\
  position: absolute;\n\
  content: '';\n\
}\n\
@-webkit-keyframes moving {\n\
  to {\n\
    background-position: 100vmin 20vmin, -100vmin -25vmin;\n\
  }\n\
}\n\
@keyframes moving {\n\
  to {\n\
    background-position: 100vmin 20vmin, -100vmin -25vmin;\n\
  }\n\
}\n\
\n\
.text {\n\
  font: 400 5vmin 'Courgette';\n\
}\n\
.text span {\n\
  font-size: 10vmin;\n\
}\n\
\n\
</style>\n\
</head>\n\
<body>\n\
	<div class='number'>404</div>\n\
	<div class='text'><span>Ooops...</span><br>page not found</div>\n\
</body>\n\
</html>";

}

ErrorString::ErrorString( const ErrorString &rhs )
{
	*this = rhs;
}

ErrorString &ErrorString::operator=( const ErrorString &rhs )
{
	if (this != &rhs)
		_errorFile = rhs._errorFile;
	return (*this);
}

ErrorString::~ErrorString( void ) { }

std::string ErrorString::getFileInString( void ) const
{
	return _errorFile;
}

void ErrorString::setErrorFile( int sCode, const std::string &errorMessage )
{
	int index2;
	std::string s;
	std::stringstream stringCode;

	stringCode << sCode;
	s = stringCode.str();
	this->_errorFile[1536] = s[0];
	this->_errorFile[1537] = s[1];
	this->_errorFile[1538] = s[2];
	index2 = _errorFile.find('<', 1590);
	_errorFile.replace(1590, index2 - 1590 , errorMessage);
}