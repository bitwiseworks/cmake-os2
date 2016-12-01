/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmExportLibraryDependenciesCommand_h
#define cmExportLibraryDependenciesCommand_h

#include "cmCommand.h"

class cmExportLibraryDependenciesCommand : public cmCommand
{
public:
  cmTypeMacro(cmExportLibraryDependenciesCommand, cmCommand);
  cmCommand* Clone() CM_OVERRIDE
  {
    return new cmExportLibraryDependenciesCommand;
  }
  bool InitialPass(std::vector<std::string> const& args,
                   cmExecutionStatus& status) CM_OVERRIDE;
  std::string GetName() const CM_OVERRIDE
  {
    return "export_library_dependencies";
  }

  void FinalPass() CM_OVERRIDE;
  bool HasFinalPass() const CM_OVERRIDE { return true; }

private:
  std::string Filename;
  bool Append;
  void ConstFinalPass() const;
};

#endif
