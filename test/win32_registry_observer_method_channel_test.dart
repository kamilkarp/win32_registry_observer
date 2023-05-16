import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:win32_registry_observer/win32_registry_observer_method_channel.dart';

void main() {
  MethodChannelWin32RegistryObserver platform = MethodChannelWin32RegistryObserver();
  const MethodChannel channel = MethodChannel('win32_registry_observer');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
