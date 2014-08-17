//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.cxx ---
//
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "LoadShaders.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//----------------------------------------------------------------------------

static const GLchar*
ReadShader( const char* filename )
{
#ifdef WIN32
	FILE* infile;
	fopen_s( &infile, filename, "rb" );
#else
    FILE* infile = fopen( filename, "rb" );
#endif // WIN32

    if ( !infile ) {
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    GLchar* source = new GLchar[len+1];

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return const_cast<const GLchar*>(source);
}

//Alternate shader file parser that treats '#include <>' and '#include ""' like include in C++
static GLchar* ReadShaderInclude(const char* fileName, std::vector<std::string>* loadedFileList = NULL)
{
	std::ifstream inFile(fileName);
	
	if ( !inFile ) {
		std::cerr << "Unable to open file '" << fileName << "'" << std::endl;
		assert(false);
        return NULL;
    }

	bool top = false;

	if (!loadedFileList)
	{
		top = true;
		loadedFileList = new std::vector<std::string>();
		loadedFileList->push_back(fileName);
	}

	std::stringstream fullCode;
	std::string currentLine;

	while (std::getline(inFile, currentLine))
	{
		std::string subString = currentLine.substr(0, 10);

		if (subString == "#include <" || subString == "#include \"")
		{
			std::string includePath = currentLine.substr(10, currentLine.length() - 11);
			bool keyFound = false;

			//Forcing #pragma once emulation
			for (auto it = loadedFileList->begin(); it != loadedFileList->end(); ++it)
			{
				if ((*it) == includePath)
				{
					keyFound = true;
					break;
				}
			}
			
			if (!keyFound)
			{
				loadedFileList->push_back(includePath);

				const char* includeSource = ReadShaderInclude(includePath.c_str(), loadedFileList);
				fullCode << includeSource << std::endl;

				delete [] includeSource;
			}
		}
		else
			fullCode << currentLine << std::endl;
	}

	inFile.close();

	std::string finalString = fullCode.str();

	GLchar* source = new GLchar[finalString.length() + 1];
	source[finalString.length()] = '\0';

	std::copy(finalString.begin(), finalString.end(), source);

	if (top)
		delete loadedFileList;

	return source;
}

//----------------------------------------------------------------------------

GLuint
LoadShaders( ShaderInfo* shaders )
{
    if ( shaders == NULL ) { return 0; }

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while ( entry->type != GL_NONE ) {
        GLuint shader = glCreateShader( entry->type );

        entry->shader = shader;

		const GLchar* source = ReadShaderInclude(entry->filename);

        //const GLchar* source = ReadShader( entry->filename );
        if ( source == NULL ) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource( shader, 1, &source, NULL );
        delete [] source;

        glCompileShader( shader );

		//Print warnings and errors from compiling
		GLsizei len;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );
		if (len > 1)
		{
			GLchar* log = new GLchar[len+1];
			glGetShaderInfoLog( shader, len, &len, log );

			std::cerr << entry->filename << ":\n" << log << std::endl;

			delete [] log;
		}

		//Check if compiling failed
        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            std::cerr << "Shader compilation failed.\n" << std::endl;

            return 0;
        }

        glAttachShader( program, shader );
        
        ++entry;
    }
    
    glLinkProgram( program );

	//Print warnings and errors from linking
	GLsizei len;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );
	if (len > 1)
	{
		GLchar* log = new GLchar[len+1];
		glGetProgramInfoLog( program, len, &len, log );

		std::cerr << log << std::endl;

		delete [] log;
	}

	//Check if linking failed
    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        
        std::cerr << "Shader linking failed." << std::endl;

        for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }
        
        return 0;
    }

    return program;
}

//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus


