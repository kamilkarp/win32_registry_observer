import 'package:flutter_test/flutter_test.dart';
import 'package:win32_registry_observer/win32_registry_observer.dart';
import 'package:win32_registry_observer/win32_registry_observer_platform_interface.dart';
import 'package:win32_registry_observer/win32_registry_observer_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockWin32RegistryObserverPlatform
    with MockPlatformInterfaceMixin
    implements Win32RegistryObserverPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final Win32RegistryObserverPlatform initialPlatform = Win32RegistryObserverPlatform.instance;

  test('$MethodChannelWin32RegistryObserver is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelWin32RegistryObserver>());
  });

  test('getPlatformVersion', () async {
    Win32RegistryObserver win32RegistryObserverPlugin = Win32RegistryObserver();
    MockWin32RegistryObserverPlatform fakePlatform = MockWin32RegistryObserverPlatform();
    Win32RegistryObserverPlatform.instance = fakePlatform;

    expect(await win32RegistryObserverPlugin.getPlatformVersion(), '42');
  });
}
