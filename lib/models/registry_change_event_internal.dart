import 'package:win32_registry_observer/models/registry_predefined_key.dart';

class RegistryChangeEventInternal {
  RegistryChangeEventInternal({
    required this.predefinedKey,
    required this.path,
    required this.includeSubkeys,
  });

  final RegistryPredefinedKey predefinedKey;
  final String path;
  final bool includeSubkeys;
}
