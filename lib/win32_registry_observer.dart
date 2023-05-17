import 'dart:collection';

import 'package:rxdart/rxdart.dart';
import 'package:win32_registry_observer/models/registry_change_event_internal.dart';

import 'models/observe_registry_change_request.dart';
import 'models/registry_change_event.dart';
import 'win32_registry_observer_platform_interface.dart';

export 'models/observe_registry_change_request.dart';
export 'models/registry_change_event.dart';
export 'models/registry_predefined_key.dart';

class Win32RegistryObserver {
  Win32RegistryObserver._() {
    _init();
  }

  static final Win32RegistryObserver _instance = Win32RegistryObserver._();
  static Win32RegistryObserver get instance => _instance;

  final _registryChangeSubject = PublishSubject<RegistryChangeEventInternal>();
  final _observersPerRequestMap = HashMap<ObserveRegistryChangeRequest, int>();

  void _init() {
    Win32RegistryObserverPlatform.instance
        .observeRegistryChanges()
        .listen((event) => _registryChangeSubject.add(event));
  }

  Stream<RegistryChangeEvent> observeRegistryKey(
      ObserveRegistryChangeRequest request) {
    _addObserverAndStartObservingIfNoObservers(request);

    return _registryChangeSubject
        .asyncExpand((event) {
          if (event.path != request.path ||
              event.includeSubkeys != request.includeSubkeys ||
              event.predefinedKey != request.predefinedKey) {
            return const Stream<RegistryChangeEvent>.empty();
          }

          return Stream.value(RegistryChangeEvent(
            predefinedKey: event.predefinedKey,
            path: event.path,
          ));
        })
        .doOnCancel(() => _removeObserverAndStopObservingIfNoObservers(request))
        .doOnDone(() => _removeObserverAndStopObservingIfNoObservers(request))
        .doOnError(
            (_, __) => _removeObserverAndStopObservingIfNoObservers(request));
  }

  void _addObserverAndStartObservingIfNoObservers(
    ObserveRegistryChangeRequest request,
  ) {
    final previousObservers = _observersPerRequestMap[request] ?? 0;

    if (previousObservers <= 0) {
      Win32RegistryObserverPlatform.instance.startObservingRegistryKey(request);
    }

    _observersPerRequestMap[request] = previousObservers + 1;
  }

  void _removeObserverAndStopObservingIfNoObservers(
    ObserveRegistryChangeRequest request,
  ) {
    final previousObservers = _observersPerRequestMap[request] ?? 0;

    if (previousObservers <= 0) {
      return;
    }

    final currentObservers = previousObservers - 1;
    _observersPerRequestMap[request] = currentObservers;

    if (currentObservers <= 0) {
      Win32RegistryObserverPlatform.instance.stopObservingRegistryKey(request);
    }
  }
}
