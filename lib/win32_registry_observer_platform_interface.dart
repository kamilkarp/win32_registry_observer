import 'package:plugin_platform_interface/plugin_platform_interface.dart';
import 'package:win32_registry_observer/models/observe_registry_change_request.dart';
import 'package:win32_registry_observer/models/registry_change_event_internal.dart';

import 'win32_registry_observer_method_channel.dart';

abstract class Win32RegistryObserverPlatform extends PlatformInterface {
  /// Constructs a Win32RegistryObserverPlatform.
  Win32RegistryObserverPlatform() : super(token: _token);

  static final Object _token = Object();

  static Win32RegistryObserverPlatform _instance =
      MethodChannelWin32RegistryObserver();

  /// The default instance of [Win32RegistryObserverPlatform] to use.
  ///
  /// Defaults to [MethodChannelWin32RegistryObserver].
  static Win32RegistryObserverPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [Win32RegistryObserverPlatform] when
  /// they register themselves.
  static set instance(Win32RegistryObserverPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<bool?> startObservingRegistryKey(
      ObserveRegistryChangeRequest request) {
    throw UnimplementedError(
        'startObservingRegistryKey() has not been implemented.');
  }

  Future<bool?> stopObservingRegistryKey(ObserveRegistryChangeRequest request) {
    throw UnimplementedError(
        'stopObservingRegistryKey() has not been implemented.');
  }

  Stream<RegistryChangeEventInternal> observeRegistryChanges() {
    throw UnimplementedError(
        'observeRegistryChanges() has not been implemented.');
  }
}
