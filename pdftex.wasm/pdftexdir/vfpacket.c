/*
Copyright 1996-2014 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ptexlib.h"

typedef struct {
    char *dataptr;
    integer len;
} packet_entry;

/* define packet_ptr, packet_array & packet_limit */
define_array(packet);

typedef struct {
    char **data;
    int *len;
    int char_count;
} vf_entry;

/* define vf_ptr, vf_array & vf_limit */
define_array(vf);

static char *packet_data_ptr;

integer newvfpacket(internalfontnumber f)
{
    int i, n = fontec[f] - fontbc[f] + 1;
    alloc_array(vf, 1, SMALL_ARRAY_SIZE);
    vf_ptr->len = xtalloc(n, int);
    vf_ptr->data = xtalloc(n, char *);
    vf_ptr->char_count = n;
    for (i = 0; i < n; i++) {
        vf_ptr->data[i] = NULL;
        vf_ptr->len[i] = 0;
    }
    return vf_ptr++ - vf_array;
}

void storepacket(internalfontnumber f, integer c, strnumber s)
{
    int l = strstart[s + 1] - strstart[s];
    vf_array[vfpacketbase[f]].len[c - fontbc[f]] = l;
    vf_array[vfpacketbase[f]].data[c - fontbc[f]] = xtalloc(l, char);
    memcpy((void *) vf_array[vfpacketbase[f]].data[c - fontbc[f]],
           (void *) (strpool + strstart[s]), (unsigned) l);
}

void startpacket(internalfontnumber f, eightbits c)
{
    packet_data_ptr = vf_array[vfpacketbase[f]].data[c - fontbc[f]];
    vfpacketlength = vf_array[vfpacketbase[f]].len[c - fontbc[f]];
}

eightbits packetbyte(void)
{
    vfpacketlength--;
    return *packet_data_ptr++;
}

void pushpacketstate(void)
{
    alloc_array(packet, 1, SMALL_ARRAY_SIZE);
    packet_ptr->dataptr = packet_data_ptr;
    packet_ptr->len = vfpacketlength;
    packet_ptr++;
}

void poppacketstate(void)
{
    if (packet_ptr == packet_array)
        pdftex_fail("packet stack empty, impossible to pop");
    packet_ptr--;
    packet_data_ptr = packet_ptr->dataptr;
    vfpacketlength = packet_ptr->len;
}

void vf_free(void)
{
    vf_entry *v;
    char **p;
    if (vf_array != NULL) {
        for (v = vf_array; v < vf_ptr; v++) {
            xfree(v->len);
            for (p = v->data; p - v->data < v->char_count; p++)
                xfree(*p);
            xfree(v->data);
        }
        xfree(vf_array);
    }
    xfree(packet_array);
}
