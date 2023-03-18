#include "ErrorString.class.hpp"

ErrorString::ErrorString( void )
{
	_errorFile = "<!DOCTYPE html>\n<html>\n<head>\n\
	<title>My Simple Website</title>\n\
	<style>\n\
	  body {\n\
	  background-color: #D9A708;\n\
	  margin: 0;\n\
	  padding: 0;\n\
	  font-family: 'Roboto', sans-serif;\n\
	}\n\
	\n\
	.center {\n\
	  position: absolute;\n\
	  top: 50%;\n\
	  left: 50%;\n\
	  transform: translate(-50%,-50%)\n\
	\n\
	}\n\
	\n\
	.circle {\n\
	  width: 20px;\n\
	  height: 20px;\n\
	  background-color: #FFF;\n\
	  border-radius: 50%;\n\
	  margin: 0 auto;\n\
	  box-shadow: 0 0 1px rgba(0,0,0,.3)\n\
	}\n\
	\n\
	.line {\n\
	  content: '';\n\
	  display: flex;\n\
	  height: 300px;\n\
	  width: 2px;\n\
	  background-color: #FFF;\n\
	  margin: 0 auto;\n\
	  transform-origin: center;\n\
	  animation: animate 3s linear infinite;\n\
	}\n\
	\n\
	.banner {\n\
	  width: 270px;\n\
	  height: 90px;\n\
	  background-color: #FFF;\n\
	  transform-origin:top ;\n\
	  animation: animate 2.9s linear infinite;\n\
	}\n\
	\n\
	h1 {\n\
	  text-align: center;\n\
	  font-size: 3em;\n\
	  margin: 0;\n\
	  padding: 0;\n\
	  line-height: 90px;\n\
	  color: #D9A708;\n\
	}\n\
	\n\
	h2 {\n\
	  color: #FFF\n\
	}\n\
\n\
@keyframes animate {\n\
	  0% {\n\
		transform: rotate(-2deg);\n\
	  }\n\
	  \n\
	  \n\
  \n\
	  50% {\n\
\n\
		transform: rotate(2deg);\n\
\n\
	  }\n\
	  \n\
	  100% {\n\
\n\
		transform: rotate(-2deg);\n\
\n\
	  }\n\
	}\n\
\n\
   \n\
	</style>\n\
  </head>\n\
  <body>\n\
	<div class=\"center\">\n\
	  \n\
	  <div class=\"circle\">\n\
	  \n\
	  </div>\n\
	\n\
	<div class=\"line\">\n\
	  \n\
	</div>\n\
	  \n\
	  <div class=\"banner\">\n\
		<h1>\n\
		  Oops!\n\
		</h1>\n\
	  </div>\n\
	\n\
	\n\
	<h2>\n\
	  Error 404 page not found\n\
	</h2>\n\
\n\
  </div>\n\
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

void ErrorString::setErrorFile( const std::string &errorMessage )
{
	int index1, index2;
	std::string tmpString;

	index1 = _errorFile.find("Error");
	tmpString = _errorFile.substr(index1);
	index2 = tmpString.find('<');
	_errorFile.replace(index1, index2 - 2, errorMessage);
}