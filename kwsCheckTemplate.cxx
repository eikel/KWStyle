/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckTemplate.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check if the template definition of the class follows
 *  a particular regular expression */
bool Parser::CheckTemplate(const char* regEx)
{
  m_TestsDone[TEMPLATE] = true;
  m_TestsDescription[TEMPLATE] = "Template should match the regex: ";
  m_TestsDescription[TEMPLATE] += regEx;

  itksys::RegularExpression regex(regEx);

  bool hasErrors = false;
  // We check all the templates in the file
  // Maybe we should separate the main class from the templated function
  // at some point.

  long int templatePos = m_BufferNoComment.find("template",0);
  while(templatePos != -1 ) 
    {
    // Definition is template <whatever name,whatever name2 = test, ...>
    long int inf = m_BufferNoComment.find("<",templatePos);
    long int sup = m_BufferNoComment.find(">",inf);

    if(inf == -1 || sup == -1)
      {
      std::cout << "CheckTemplate(): There is a problem parsing the file" << std::endl;
      return false;
      }
   
    long int i = inf+1;
    bool inWord = false;
    bool afterEqual = false;
    std::string currentWord = "";
    while(i<=sup)
      {
      // If we have a space
      if(m_BufferNoComment[i] == ' ')
        {
        // do nothing
        inWord = false;
        }
      else if(m_BufferNoComment[i] == ',' || m_BufferNoComment[i] == '>' || m_BufferNoComment[i] == '=')
        {
        inWord = false;

        if(!afterEqual && !regex.find(currentWord))
          {
          Error error;
          error.line = this->GetLineNumber(i,true);
          error.line2 = error.line;
          error.number = TEMPLATE;
          error.description = "Template definition (" + currentWord + ") doesn't match regular expression";
          m_ErrorList.push_back(error);
          hasErrors = true;
          }

        if(m_BufferNoComment[i] == '=')
          {
          afterEqual = true;
          }
        else
          {
          afterEqual = false;
          }
        }
      else
        {
        if(!inWord)
          {
          currentWord = "";
          }
        currentWord += m_BufferNoComment[i];
        inWord = true;
        }
      i++;
      }

    templatePos = m_BufferNoComment.find("template",templatePos+1);  
    }

  return !hasErrors;
}

} // end namespace kws
