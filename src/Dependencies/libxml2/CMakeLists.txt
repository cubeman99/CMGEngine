
add_definitions(-DLIBXML_STATIC)

set(BASE_SRCS
	buf.c
	c14n.c
	catalog.c
	chvalid.c
	debugXML.c
	dict.c
	DOCBparser.c
	encoding.c
	entities.c
	error.c
	globals.c
	hash.c
	HTMLparser.c
	HTMLtree.c
	legacy.c
	list.c
	nanoftp.c
	nanohttp.c
	parser.c
	parserInternals.c
	pattern.c
	relaxng.c
	runsuite.c
	runtest.c
	runxmlconf.c
	SAX.c
	SAX2.c
	schematron.c
	testapi.c
	testAutomata.c
	testC14N.c
	testchar.c
	testdict.c
	testdso.c
	testHTML.c
	testlimits.c
	testModule.c
	testReader.c
	testrecurse.c
	testRegexp.c
	testRelax.c
	testSAX.c
	testSchemas.c
	testThreads.c
	testURI.c
	testXPath.c
	threads.c
	tree.c
	trio.c
	trionan.c
	triostr.c
	uri.c
	valid.c
	xinclude.c
	xlink.c
	xmlcatalog.c
	xmlIO.c
	xmllint.c
	xmlmemory.c
	xmlmodule.c
	xmlreader.c
	xmlregexp.c
	xmlsave.c
	xmlschemas.c
	xmlschemastypes.c
	xmlstring.c
	xmlunicode.c
	xmlwriter.c
	xpath.c
	xpointer.c
	xzlib.c
)

# Specify library include directories
include_directories(include)
include_directories(.)

cmg_install_dependency(libxml2 "${BASE_SRCS}")

target_link_libraries(libxml2
	zlib
	libiconv
)

include_directories("../libiconv/include")
include_directories("../zlib")

install(DIRECTORY "include/libxml" DESTINATION "include")
