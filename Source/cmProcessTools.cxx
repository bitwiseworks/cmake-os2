/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmProcessTools.h"

#include <cmsys/Process.h>
#include <ostream>

void cmProcessTools::RunProcess(struct cmsysProcess_s* cp, OutputParser* out,
                                OutputParser* err)
{
  cmsysProcess_Execute(cp);
  char* data = CM_NULLPTR;
  int length = 0;
  int p;
  while ((out || err) &&
         (p = cmsysProcess_WaitForData(cp, &data, &length, CM_NULLPTR), p)) {
    if (out && p == cmsysProcess_Pipe_STDOUT) {
      if (!out->Process(data, length)) {
        out = CM_NULLPTR;
      }
    } else if (err && p == cmsysProcess_Pipe_STDERR) {
      if (!err->Process(data, length)) {
        err = CM_NULLPTR;
      }
    }
  }
  cmsysProcess_WaitForExit(cp, CM_NULLPTR);
}

cmProcessTools::LineParser::LineParser(char sep, bool ignoreCR)
  : Log(CM_NULLPTR)
  , Prefix(CM_NULLPTR)
  , Separator(sep)
  , LineEnd('\0')
  , IgnoreCR(ignoreCR)
{
}

void cmProcessTools::LineParser::SetLog(std::ostream* log, const char* prefix)
{
  this->Log = log;
  this->Prefix = prefix ? prefix : "";
}

bool cmProcessTools::LineParser::ProcessChunk(const char* first, int length)
{
  const char* last = first + length;
  for (const char* c = first; c != last; ++c) {
    if (*c == this->Separator || *c == '\0') {
      this->LineEnd = *c;

      // Log this line.
      if (this->Log && this->Prefix) {
        *this->Log << this->Prefix << this->Line << "\n";
      }

      // Hand this line to the subclass implementation.
      if (!this->ProcessLine()) {
        this->Line = "";
        return false;
      }

      this->Line = "";
    } else if (*c != '\r' || !this->IgnoreCR) {
      // Append this character to the line under construction.
      this->Line.append(1, *c);
    }
  }
  return true;
}
