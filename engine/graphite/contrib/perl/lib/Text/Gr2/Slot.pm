=head1 NAME

Text::Gr2::Slot - Represents a slot within a segment

=head1 METHODS

=head2 advance_X

Returns the advance (width) of this slot.

=head2 advance_Y

Returns the Y advance (height) of this slot.

=head2 origin_X

Returns the X co-ordinate of the start of this slot.

=head2 origin_Y

Returns the Y co-ordinate of the start of this slot.

=head2 original

Returns the CharInfo which represents this slot, to be retrieved with
C<< $seg->cinfo($slot->original()) >>.

=head2 after

Returns the CharInfo after us, to be retrieved with
C<< $seg->cinfo($slot->after()) >>.

=head2 before

Returns the CharInfo before us, to be retrieved with
C<< $seg->cinfo($slot->before()) >>.

=head2 next_in_segment

Returns the next slot in this segment.

=head2 prev_in_segment

Returns the previous slot in this segment.

=head2 attached_to

Returns the parent slot of this slot, if this slot is attached to
another. (If I am the first accent on a main character with two accents,
this will return the main character.)

=head2 next_sibling_attachment

If we are attached to another slot, return the next slot along which is
also attached to the same slot. (If I am the first accent on a main
character with two accents, this will return the second accent.)

=head2 first_attachment

Returns the first slot attached to this one. (If I am the main character
and I have two accents, this will return the first accent.)

=head2 gid

Returns the glyph ID of the glyph in this slot.

=head1 INTERNAL METHODS

=head2 attr

Returns a slot attribute value.

=head2 can_insert_before

Returns whether text may be inserted before this glyph [check this isn't
inverted]

=cut

1;
