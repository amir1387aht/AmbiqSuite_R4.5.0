# *****************************************************************************
#
#    keil_link.py
#
#    @brief Generate the Keil link control file.
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2024, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
#
# *****************************************************************************

from string import Template
import textwrap


def generate_link_script(config):

    mapping = dict()

    mapping['ro_base'] = format_number(config['MCU_MRAM']['start'])
    mapping['rw_base'] = format_number(config['MCU_TCM']['start'])
    mapping['shared_base'] = format_number(config['SHARED_SRAM']['start'])

    mapping['ro_size'] = format_number(config['MCU_MRAM']['length'])
    mapping['rw_size'] = format_number(config['MCU_TCM']['length'])
    mapping['shared_size'] = format_number(config['SHARED_SRAM']['length'])

    mapping['additional_sections'] = generate_sections(config)

    return link_script_template.substitute(**mapping)


def generate_sections(config):
    # If there aren't any custom sections in the config file, we don't need to
    # add anything to the linker scripts.
    if 'custom_sections' not in config:
        return ''
    elif not config['custom_sections']:
        return ''

    L = []
    for mem_section in config['custom_sections']:
        D = dict()
        D['name'] = mem_section['blockname']
        D['start'] = format_number(mem_section['start'])
        D['length'] = format_number(mem_section['length'])
        D['sections'] = '\n'.join('    * ({})'.format(x) for x in mem_section['sections'])

        S = extra_section_template.substitute(**D)
        L.append(textwrap.indent(S, 4 * ' '))

    return '\n' + '\n'.join(L)


def format_number(n):
    return '0x{:08X}'.format(n)


link_script_template = Template('''\
//******************************************************************************
//
// Linker file for Segger Embedded Studio linker configuration.
//
//******************************************************************************

// Define Memory Regions
define memory with size = 4G; 
define region MCU_MRAM      = [from ${ro_base} size ${ro_size}];
define region MCU_TCM       = [from ${rw_base} size ${rw_size}];
define region SHARED_SRAM   = [from ${shared_base} size ${shared_size}];

// Thread-local-storage load image
define block tbss                           { section .tbss,      section .tbss.*  };
define block tdata                          { section .tdata,     section .tdata.* };
define block tls with fixed order           { block tbss, block tdata };
define block tdata_load                     { copy of block tdata };

// Define Blocks
define block heap  with size =    0, alignment = 8, readwrite access { };
define block stack with size = 2048, alignment = 8, readwrite access { };
define block shared                         { section .shared,      section .shared.*   };

define block FLASHBASE                      { section .patch };

// Configure Initialization
do not initialize                       { section .non_init, section .non_init.*};
initialize by copy with packing=auto    { section .data, section .data.* };
initialize by symbol __SEGGER_init_heap { block heap  };

// Place Inputs
place at start of MCU_MRAM              { section .vectors, block FLASHBASE };
place in MCU_MRAM                       { readexec, readonly };
place in MCU_MRAM                       { block tdata_load };
place in MCU_TCM with auto order        { block tls, readwrite, zeroinit };
place in MCU_TCM with fixed order       { block heap, block stack };
place at start of SHARED_SRAM           { block shared };
${additional_sections}\
''')

extra_section_template = Template('''\
${name} ${start} ${length}
{
${sections}
}
''')
