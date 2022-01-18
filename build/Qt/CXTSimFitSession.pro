TEMPLATE = subdirs

SUBDIRS = CXTSimFit \
		  IBK \
		  sundials \
		  levmar

	# where to find the sub projects
CXTSimFit.file = ../../CXTSimFit/projects/Qt/CXTSimFit.pro
IBK.file = ../../externals/IBK/projects/Qt/IBK.pro
levmar.file = ../../externals/levmar/projects/Qt/levmar.pro
sundials.file = ../../externals/sundials/projects/Qt/sundials.pro

