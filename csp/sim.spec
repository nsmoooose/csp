# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['bin/sim.py'],
    pathex=[
        "csplib/.bin/",
        "cspsim/.bin/",
        "cspwf/.bin/",
        "/ucrt64/bin/",
    ],
    binaries=[
        ("csplib/.bin/csplib.dll", "."),
        ("csplib/.bin/_csplib_module.pyd", "."),
        ("cspsim/.bin/cspsim.dll", "."),
        ("cspsim/.bin/_cspsim_module.pyd", "."),
        ("cspwf/.bin/cspwf.dll", "."),
        ("modules/demeter/.bin/demeter.dll", "."),
        ("modules/chunklod/.bin/chunklod.dll", "."),
    ],
    datas=[
        ("bin/sim.ini", "csp/bin"),
        ("data", "csp/data"),
        ("tools/hid/examples", "csp/tools/hid/examples"),
        ("tools/hid/maps", "csp/tools/hid/maps"),
    ],
    hiddenimports=[
    #    "csp.csplib",
    #    "csp.cspsim",
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='sim',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='csp',
)
