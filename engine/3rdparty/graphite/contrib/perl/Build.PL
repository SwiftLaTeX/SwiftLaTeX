use strict;
use warnings;
use Module::Build;

my $builder = Module::Build->new(
    module_name         => 'Text::Gr2',
    license             => 'Perl',
    dist_author         => 'Simon Cozens <simon@cpan.org>',
    dist_version_from   => 'lib/Text/Gr2.pm',
    extra_compiler_flags => $^O ne 'MSWin32' ? '-Wall -Wno-unused-variable' : '',
    extra_linker_flags  => $^O eq 'MSWin32' ? '../../build/src/graphite2.lib' : '-lgraphite2',
    include_dirs        => ['.', '../include'],
    requires => {
        'Test::More' => 0,
    },
    add_to_cleanup      => [ 'Gr2-*' ],
);

$builder->create_build_script();
