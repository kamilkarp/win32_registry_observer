import 'package:win32_registry_observer/models/registry_predefined_key.dart';

class RegistryChangeEvent {
  RegistryChangeEvent({
    required this.predefinedKey,
    required this.path,
  });

  final RegistryPredefinedKey predefinedKey;
  final String path;
}
