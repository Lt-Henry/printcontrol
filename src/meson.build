
cpp = meson.get_compiler('cpp')
be = cpp.find_library('be')
tracker = cpp.find_library('tracker')
translation = cpp.find_library('translation')
device = cpp.find_library('device')

executable('PrintControl', ['main.cpp','PrintControl.cpp','MainWindow.cpp','SettingsWindow.cpp','SerialDriver.cpp','GCode.cpp','Settings.cpp'],
	dependencies:[be,tracker,translation,device]
	)
