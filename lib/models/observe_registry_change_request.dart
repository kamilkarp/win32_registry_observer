import 'package:win32_registry_observer/models/registry_predefined_key.dart';

class ObserveRegistryChangeRequest {
  ObserveRegistryChangeRequest({
    required this.predefinedKey,
    required this.path,
    this.includeSubkeys = false,
  });

  final RegistryPredefinedKey predefinedKey;
  final String path;
  final bool includeSubkeys;

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is ObserveRegistryChangeRequest &&
          runtimeType == other.runtimeType &&
          predefinedKey == other.predefinedKey &&
          path == other.path &&
          includeSubkeys == other.includeSubkeys;

  @override
  int get hashCode =>
      predefinedKey.hashCode ^ path.hashCode ^ includeSubkeys.hashCode;

  ObserveRegistryChangeRequest copyWith({
    RegistryPredefinedKey? predefinedKey,
    String? path,
    bool? includeSubkeys,
  }) {
    return ObserveRegistryChangeRequest(
      predefinedKey: predefinedKey ?? this.predefinedKey,
      path: path ?? this.path,
      includeSubkeys: includeSubkeys ?? this.includeSubkeys,
    );
  }
}
