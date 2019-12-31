use PDF::API2;
use Text::Gr2;
use File::Slurp;
my ($fontfile, $textfile) = @ARGV;

push @PDF::API2::FontDirs, "/Library/Fonts", "/Users/simon/Library/Fonts";

my $graf = read_file($textfile);
my $face = Text::Gr2::Face->open("/Users/simon/Library/Fonts/$fontfile",
    cache_size => 1000,
    preload_glyphs => 1);
die "Invalid font, failed to read or parse tables\n" unless $face;

my $sized_font = $face->make_font(12);
my $pdf = PDF::API2->new(-file => "test.pdf");
$pdf->mediabox("A4");
my $page = $pdf->page;
our $font = $pdf->ttfont($fontfile) || die "No PDF font found";
my $text = $page->text;
$text->font($font,12);

my $seg = $sized_font->segment($face, $graf);
my @slots = $seg->slots;
my @cinfos = $seg->cinfos;
for (0..$#slots) {
    $text->translate(20 + $slots[$_]->origin_X,200 + $slots[$_]->origin_Y);
    print $slots[$_]->origin_X,"\t", $cinfos[$_]->unicode_char,"\n";
    $text->text(chr($cinfos[$_]->unicode_char()));
}
$pdf->save;
