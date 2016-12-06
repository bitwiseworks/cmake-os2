/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmGlobalXCodeGenerator_h
#define cmGlobalXCodeGenerator_h

#include "cmGlobalGenerator.h"

#include "cmCustomCommand.h"
#include "cmXCodeObject.h"
class cmGlobalGeneratorFactory;
class cmSourceFile;
class cmSourceGroup;

/** \class cmGlobalXCodeGenerator
 * \brief Write a Unix makefiles.
 *
 * cmGlobalXCodeGenerator manages UNIX build process for a tree
 */
class cmGlobalXCodeGenerator : public cmGlobalGenerator
{
public:
  cmGlobalXCodeGenerator(cmake* cm, std::string const& version);
  static cmGlobalGeneratorFactory* NewFactory();

  ///! Get the name for the generator.
  virtual std::string GetName() const
  {
    return cmGlobalXCodeGenerator::GetActualName();
  }
  static std::string GetActualName() { return "Xcode"; }

  /** Get the documentation entry for this generator.  */
  static void GetDocumentation(cmDocumentationEntry& entry);

  ///! Create a local generator appropriate to this Global Generator
  virtual cmLocalGenerator* CreateLocalGenerator(cmMakefile* mf);

  /**
   * Try to determine system information such as shared library
   * extension, pthreads, byte order etc.
   */
  virtual void EnableLanguage(std::vector<std::string> const& languages,
                              cmMakefile*, bool optional);
  /**
   * Try running cmake and building a file. This is used for dynalically
   * loaded commands, not as part of the usual build process.
   */
  virtual void GenerateBuildCommand(
    std::vector<std::string>& makeCommand, const std::string& makeProgram,
    const std::string& projectName, const std::string& projectDir,
    const std::string& targetName, const std::string& config, bool fast,
    bool verbose,
    std::vector<std::string> const& makeOptions = std::vector<std::string>());

  /** Append the subdirectory for the given configuration.  */
  virtual void AppendDirectoryForConfig(const std::string& prefix,
                                        const std::string& config,
                                        const std::string& suffix,
                                        std::string& dir);

  virtual void FindMakeProgram(cmMakefile*);

  ///! What is the configurations directory variable called?
  virtual const char* GetCMakeCFGIntDir() const;
  ///! expand CFGIntDir
  virtual std::string ExpandCFGIntDir(const std::string& str,
                                      const std::string& config) const;

  void SetCurrentLocalGenerator(cmLocalGenerator*);

  /** Return true if the generated build tree may contain multiple builds.
      i.e. "Can I build Debug and Release in the same tree?" */
  virtual bool IsMultiConfig() const;

  virtual bool SetGeneratorToolset(std::string const& ts, cmMakefile* mf);
  void AppendFlag(std::string& flags, std::string const& flag);

protected:
  virtual void AddExtraIDETargets();
  virtual void Generate();

private:
  cmXCodeObject* CreateOrGetPBXGroup(cmGeneratorTarget* gtgt,
                                     cmSourceGroup* sg);
  cmXCodeObject* CreatePBXGroup(cmXCodeObject* parent, std::string name);
  bool CreateGroups(std::vector<cmLocalGenerator*>& generators);
  std::string XCodeEscapePath(const std::string& p);
  std::string RelativeToSource(const char* p);
  std::string RelativeToBinary(const char* p);
  std::string ConvertToRelativeForMake(const char* p);
  void CreateCustomCommands(cmXCodeObject* buildPhases,
                            cmXCodeObject* sourceBuildPhase,
                            cmXCodeObject* headerBuildPhase,
                            cmXCodeObject* resourceBuildPhase,
                            std::vector<cmXCodeObject*> contentBuildPhases,
                            cmXCodeObject* frameworkBuildPhase,
                            cmGeneratorTarget* gtgt);

  std::string ComputeInfoPListLocation(cmGeneratorTarget* target);

  void AddCommandsToBuildPhase(cmXCodeObject* buildphase,
                               cmGeneratorTarget* target,
                               std::vector<cmCustomCommand> const& commands,
                               const char* commandFileName);

  void CreateCustomRulesMakefile(const char* makefileBasename,
                                 cmGeneratorTarget* target,
                                 std::vector<cmCustomCommand> const& commands,
                                 const std::string& configName);

  cmXCodeObject* FindXCodeTarget(const cmGeneratorTarget*);
  std::string GetOrCreateId(const std::string& name, const std::string& id);

  // create cmXCodeObject from these functions so that memory can be managed
  // correctly.  All objects created are stored in this->XCodeObjects.
  cmXCodeObject* CreateObject(cmXCodeObject::PBXType ptype);
  cmXCodeObject* CreateObject(cmXCodeObject::Type type);
  cmXCodeObject* CreateString(const std::string& s);
  cmXCodeObject* CreateObjectReference(cmXCodeObject*);
  cmXCodeObject* CreateFlatClone(cmXCodeObject*);
  cmXCodeObject* CreateXCodeTarget(cmGeneratorTarget* gtgt,
                                   cmXCodeObject* buildPhases);
  void ForceLinkerLanguages();
  void ForceLinkerLanguage(cmGeneratorTarget* gtgt);
  const char* GetTargetLinkFlagsVar(const cmGeneratorTarget* target) const;
  const char* GetTargetFileType(cmGeneratorTarget* target);
  const char* GetTargetProductType(cmGeneratorTarget* target);
  std::string AddConfigurations(cmXCodeObject* target,
                                cmGeneratorTarget* gtgt);
  void AppendOrAddBuildSetting(cmXCodeObject* settings, const char* attr,
                               const char* value);
  void AppendBuildSettingAttribute(cmXCodeObject* target, const char* attr,
                                   const char* value,
                                   const std::string& configName);
  cmXCodeObject* CreateUtilityTarget(cmGeneratorTarget* gtgt);
  void AddDependAndLinkInformation(cmXCodeObject* target);
  void CreateBuildSettings(cmGeneratorTarget* gtgt,
                           cmXCodeObject* buildSettings,
                           const std::string& buildType);
  std::string ExtractFlag(const char* flag, std::string& flags);
  std::string ExtractFlagRegex(const char* exp, int matchIndex,
                               std::string& flags);
  void FilterConfigurationAttribute(std::string const& configName,
                                    std::string& attribute);
  void SortXCodeObjects();
  // delete all objects in the this->XCodeObjects vector.
  void ClearXCodeObjects();
  bool CreateXCodeObjects(cmLocalGenerator* root,
                          std::vector<cmLocalGenerator*>& generators);
  void OutputXCodeProject(cmLocalGenerator* root,
                          std::vector<cmLocalGenerator*>& generators);
  void WriteXCodePBXProj(std::ostream& fout, cmLocalGenerator* root,
                         std::vector<cmLocalGenerator*>& generators);
  cmXCodeObject* CreateXCodeFileReferenceFromPath(const std::string& fullpath,
                                                  cmGeneratorTarget* target,
                                                  const std::string& lang,
                                                  cmSourceFile* sf);
  cmXCodeObject* CreateXCodeSourceFileFromPath(const std::string& fullpath,
                                               cmGeneratorTarget* target,
                                               const std::string& lang,
                                               cmSourceFile* sf);
  cmXCodeObject* CreateXCodeFileReference(cmSourceFile* sf,
                                          cmGeneratorTarget* target);
  cmXCodeObject* CreateXCodeSourceFile(cmLocalGenerator* gen, cmSourceFile* sf,
                                       cmGeneratorTarget* gtgt);
  bool CreateXCodeTargets(cmLocalGenerator* gen, std::vector<cmXCodeObject*>&);
  bool IsHeaderFile(cmSourceFile*);
  void AddDependTarget(cmXCodeObject* target, cmXCodeObject* dependTarget);
  void CreateXCodeDependHackTarget(std::vector<cmXCodeObject*>& targets);
  bool SpecialTargetEmitted(std::string const& tname);
  void SetGenerationRoot(cmLocalGenerator* root);
  void AddExtraTargets(cmLocalGenerator* root,
                       std::vector<cmLocalGenerator*>& gens);
  cmXCodeObject* CreateBuildPhase(const char* name, const char* name2,
                                  cmGeneratorTarget* target,
                                  const std::vector<cmCustomCommand>&);
  void CreateReRunCMakeFile(cmLocalGenerator* root,
                            std::vector<cmLocalGenerator*> const& gens);

  std::string LookupFlags(const std::string& varNamePrefix,
                          const std::string& varNameLang,
                          const std::string& varNameSuffix,
                          const std::string& default_flags);

  class Factory;
  class BuildObjectListOrString;
  friend class BuildObjectListOrString;

  void AppendDefines(BuildObjectListOrString& defs, const char* defines_list,
                     bool dflag = false);
  void AppendDefines(BuildObjectListOrString& defs,
                     std::vector<std::string> const& defines,
                     bool dflag = false);

  void ComputeTargetObjectDirectory(cmGeneratorTarget* gt) const;

protected:
  virtual const char* GetInstallTargetName() const { return "install"; }
  virtual const char* GetPackageTargetName() const { return "package"; }

  unsigned int XcodeVersion;
  std::string VersionString;
  std::set<std::string> XCodeObjectIDs;
  std::vector<cmXCodeObject*> XCodeObjects;
  cmXCodeObject* RootObject;

private:
  std::string const& GetXcodeBuildCommand();
  std::string FindXcodeBuildCommand();
  std::string XcodeBuildCommand;
  bool XcodeBuildCommandInitialized;

  void PrintCompilerAdvice(std::ostream&, std::string const&,
                           const char*) const
  {
  }

  std::string GetObjectsNormalDirectory(const std::string& projName,
                                        const std::string& configName,
                                        const cmGeneratorTarget* t) const;

  void addObject(cmXCodeObject* obj);
  std::string PostBuildMakeTarget(std::string const& tName,
                                  std::string const& configName);
  cmXCodeObject* MainGroupChildren;
  cmXCodeObject* SourcesGroupChildren;
  cmXCodeObject* ResourcesGroupChildren;
  cmMakefile* CurrentMakefile;
  cmLocalGenerator* CurrentLocalGenerator;
  std::vector<std::string> CurrentConfigurationTypes;
  std::string CurrentReRunCMakeMakefile;
  std::string CurrentXCodeHackMakefile;
  std::string CurrentProject;
  std::set<std::string> TargetDoneSet;
  std::vector<std::string> ProjectSourceDirectoryComponents;
  std::vector<std::string> ProjectOutputDirectoryComponents;
  std::map<std::string, cmXCodeObject*> GroupMap;
  std::map<std::string, cmXCodeObject*> GroupNameMap;
  std::map<std::string, cmXCodeObject*> TargetGroup;
  std::map<std::string, cmXCodeObject*> FileRefs;
  std::map<cmGeneratorTarget const*, cmXCodeObject*> XCodeObjectMap;
  std::vector<std::string> Architectures;
  std::string GeneratorToolset;
};

#endif