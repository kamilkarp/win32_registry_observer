import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:win32_registry_observer/models/observe_registry_change_request.dart';
import 'package:win32_registry_observer/models/registry_change_event_internal.dart';
import 'package:win32_registry_observer/models/registry_predefined_key.dart';

import 'win32_registry_observer_platform_interface.dart';

/// An implementation of [Win32RegistryObserverPlatform] that uses method channels.
class MethodChannelWin32RegistryObserver extends Win32RegistryObserverPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('win32_registry_observer');

  @visibleForTesting
  final registryEventsChannel =
      const EventChannel('win32_registry_observer/registry_change_channel');

  @override
  Future<bool?> startObservingRegistryKey(
      ObserveRegistryChangeRequest request) {
    if (Platform.isWindows) {
      return methodChannel.invokeMethod<bool>('startObservingRegistryKey', {
        'predefinedKey': request.predefinedKey.win32Value,
        'keyPath': request.path,
        'includeSubkeys': request.includeSubkeys,
      });
    } else {
      throw UnimplementedError('Only implemented on Windows');
    }
  }

  @override
  Future<bool?> stopObservingRegistryKey(ObserveRegistryChangeRequest request) {
    if (Platform.isWindows) {
      return methodChannel.invokeMethod<bool>('stopObservingRegistryKey', {
        'predefinedKey': request.predefinedKey.win32Value,
        'keyPath': request.path,
        'includeSubkeys': request.includeSubkeys,
      });
    } else {
      throw UnimplementedError('Only implemented on Windows');
    }
  }

  @override
  Stream<RegistryChangeEventInternal> observeRegistryChanges() {
    if (Platform.isWindows) {
      return registryEventsChannel
          .receiveBroadcastStream()
          .map((event) => event as Map<Object?, Object?>)
          .asyncExpand((eventMap) {
        if (!eventMap.containsKey('type') ||
            !eventMap.containsKey('path') ||
            !eventMap.containsKey('subkeysIncluded') ||
            !eventMap.containsKey('predefinedKey')) {
          return const Stream.empty();
        }

        try {
          final predefinedKey = RegistryPredefinedKey.values.firstWhere(
              (element) =>
                  element.win32Value == eventMap['predefinedKey'] as int);

          final event = RegistryChangeEventInternal(
            predefinedKey: predefinedKey,
            path: eventMap['path'] as String,
            includeSubkeys: eventMap['subkeysIncluded'] as bool,
          );

          return Stream.value(event);
        } catch (error, stackTrace) {
          return Stream.error(error, stackTrace);
        }
      });
    } else {
      throw UnimplementedError('Only implemented on Windows');
    }
  }
}
