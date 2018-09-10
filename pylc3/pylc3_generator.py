#!/usr/bin/python
from pygccxml import parser
from pygccxml import declarations
from pyplusplus import module_builder

# Create configuration for CastXML
xml_generator_config = parser.xml_generator_configuration_t(
                                    xml_generator_path='/usr/bin/castxml',
                                    xml_generator='castxml',
                                    compiler='gnu',
                                    compiler_path='/usr/bin/gcc',
                                    cflags='-std=c++11 -I../liblc3')

# List of all the C++ header of our library
header_collection = ["PyLC3.hpp", "../liblc3/lc3_all.hpp"]

# Parses the source files and creates a module_builder object
builder = module_builder.module_builder_t(
                        header_collection,
                        xml_generator_path='/usr/bin/castxml',
                        xml_generator_config=xml_generator_config)

# Debugging
#builder.print_declarations()

# Whitelist exporting of stuff.
builder.decls().exclude()

lc3_state = builder.class_('LC3State')
lc3_state.include()
builder.class_("lc3_breakpoint_info").include()
builder.class_("lc3_watchpoint_info").include()
builder.class_("lc3_blackbox_info").include()
builder.class_("lc3_subroutine_call_info").include()
builder.class_("lc3_trap_call_info").include()
builder.class_("lc3_memory_stats").include()

builder.decl("::std::vector<lc3_subroutine_call_info, std::allocator<lc3_subroutine_call_info> >").include()
builder.decl("::std::vector<lc3_trap_call_info, std::allocator<lc3_trap_call_info> >").include()
builder.decl("::std::map<unsigned short, lc3_blackbox_info, std::less<unsigned short>, std::allocator<std::pair<const unsigned short, lc3_blackbox_info> > >").include()
builder.decl("::std::map<unsigned short, lc3_breakpoint_info, std::less<unsigned short>, std::allocator<std::pair<const unsigned short, lc3_breakpoint_info> > >").include()
builder.decl("::std::map<unsigned short, lc3_watchpoint_info, std::less<unsigned short>, std::allocator<std::pair<const unsigned short, lc3_watchpoint_info> > >").include()



# Don't export accessors
builder.classes().add_properties(exclude_accessors=True)
# Enclude protected and private methods.
builder.calldefs(declarations.access_type_matcher_t('protected')).exclude()
builder.calldefs(declarations.access_type_matcher_t('private')).exclude()

# Define a name for the module
builder.build_code_creator(module_name="pylc3")

# Writes the C++ interface file
builder.write_module('PyLC3Gen.cpp')
