test_scripts = [
    'bindings/TestPythonDataSource.py'
]

foreach test:test_scripts
    test(test.split('.')[0], python3,
        args:[meson.current_source_dir()+'/'+test, '--verbose'],
        env:['PYTHONPATH='+meson.project_build_root()+'/bindings']
    )
endforeach

library('tests_fake',[],
        extra_files:test_scripts+['bindings/manual_test.py'])
