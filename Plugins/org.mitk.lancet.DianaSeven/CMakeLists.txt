
project(org_mitk_lancet_DianaSeven)

mitk_create_plugin(
  EXPORT_DIRECTIVE DIANASEVEN_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkIGTUI MitkLancetIGT MitkLancetRobot MitkLancetFileIO MitkLancetPrintDataHelper MitkLancetHardwareDevice MitkLancetRobot MitkLancetRobotRegistration
)
