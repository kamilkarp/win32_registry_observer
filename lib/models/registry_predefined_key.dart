enum RegistryPredefinedKey {
  /// Registry entries subordinate to this key define the physical state of the
  /// computer, including data about the bus type, system memory, and installed
  /// hardware and software.
  localMachine(0x80000002),

  /// Registry entries subordinate to this key define the preferences of the
  /// current user. These preferences include the settings of environment
  /// variables, data about program groups, colors, printers, network
  /// connections, and application preferences. This key makes it easier to
  /// establish the current user's settings; the key maps to the current user's
  /// branch in `HKEY_USERS`.
  currentUser(0x80000001),

  /// Registry entries subordinate to this key define the default user
  /// configuration for new users on the local computer and the user
  /// configuration for the current user.
  allUsers(0x80000003),

  /// Registry entries subordinate to this key define types (or classes) of
  /// documents and the properties associated with those types. Shell and COM
  /// applications use the information stored under this key.
  classesRoot(0x80000000),

  /// Contains information about the current hardware profile of the local
  /// computer system. The information under `HKEY_CURRENT_CONFIG` describes
  /// only the differences between the current hardware configuration and the
  /// standard configuration.
  currentConfig(0x80000005),

  /// Registry entries subordinate to this key allow you to access performance
  /// data. The data is not actually stored in the registry; the registry
  /// functions cause the system to collect the data from its source.
  performanceData(0x80000004);

  /// Returns the handle for a predefined key.
  final int win32Value;

  const RegistryPredefinedKey(this.win32Value);
}
