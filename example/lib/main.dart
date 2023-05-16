import 'dart:math';

import 'package:flutter/material.dart';
import 'package:win32_registry_observer/win32_registry_observer.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _win32RegistryObserverPlugin = Win32RegistryObserver.instance;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Column(
          children: [
            StreamBuilder(
              builder: (context, snapshot) {
                if (!snapshot.hasData) {
                  return const Text('Test No data');
                }
                return Text(Random().nextInt(500000).toString());
              },
              stream: _win32RegistryObserverPlugin.observeRegistryKey(
                ObserveRegistryChangeRequest(
                  predefinedKey: RegistryPredefinedKey.currentUser,
                  path: 'Test',
                  includeSubkeys: false,
                ),
              ),
            ),
            StreamBuilder(
              builder: (context, snapshot) {
                if (!snapshot.hasData) {
                  return const Text('Test No data');
                }
                return Text(Random().nextInt(500000).toString());
              },
              stream: _win32RegistryObserverPlugin.observeRegistryKey(
                ObserveRegistryChangeRequest(
                  predefinedKey: RegistryPredefinedKey.currentUser,
                  path: 'Test',
                ),
              ),
            ),
            StreamBuilder(
              builder: (context, snapshot) {
                if (!snapshot.hasData) {
                  return const Text('Test No data');
                }
                return Text(Random().nextInt(500000).toString());
              },
              stream: _win32RegistryObserverPlugin.observeRegistryKey(
                ObserveRegistryChangeRequest(
                  predefinedKey: RegistryPredefinedKey.currentUser,
                  path: 'Test',
                ),
              ),
            ),
            StreamBuilder(
              builder: (context, snapshot) {
                if (!snapshot.hasData) {
                  return const Text('Startup no data');
                }
                return Text(Random().nextInt(500000).toString());
              },
              stream: _win32RegistryObserverPlugin.observeRegistryKey(
                ObserveRegistryChangeRequest(
                  predefinedKey: RegistryPredefinedKey.currentUser,
                  path:
                      r'Software\Microsoft\Windows\CurrentVersion\Explorer\StartupApproved\Run',
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
