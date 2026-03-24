
# Keep in sync with the CI workflow
CPP_VERSION=20

NB_PROCS=8

debug:
	mkdir -p cmake-build/Debug && \
	cd cmake-build/Debug && \
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
		-S ../.. \
		-B . \
	&& \
	make -j ${NB_PROCS}

debugWithTests:
	mkdir -p cmake-build/Debug && \
	cd cmake-build/Debug && \
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_TESTS=ON \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
		-S ../.. \
		-B . \
	&& \
	make -j ${NB_PROCS}

release:
	mkdir -p cmake-build/Release && \
	cd cmake-build/Release && \
	cmake \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
		-S ../.. \
		-B . \
	&& \
	make -j ${NB_PROCS}

clean:
	rm -rf cmake-build

cleanSandbox:
	rm -rf sandbox

cleanCoverage:
	find . -type f -name '*.gcda' -delete

copyData:
	mkdir -p sandbox/
	rsync -avH scripts/ sandbox/

copyDebug: debug copyData
	rsync -avH cmake-build/Debug/lib cmake-build/Debug/bin sandbox/

copyRelease: release copyData
	rsync -avH cmake-build/Release/lib cmake-build/Release/bin sandbox/

runserver: copyRelease
	cd sandbox && ./run.sh swarms_server

drunserver: copyDebug
	cd sandbox && ./debug.sh swarms_server

PHONY: .tests
tests: debugWithTests copyDebug

# Use like this:
# make rununittests test_filters="TestSuite*"
# make rununittests test_filters="TestSuite*" test_repeat=10
rununittests: tests cleanCoverage
	cd sandbox && ./tests.sh unitTests $(test_filters) $(test_repeat)

runintegrationtests: tests cleanCoverage
	cd sandbox && ./tests.sh integrationTests $(test_filters) $(test_repeat)

format:
	find src/ -iname '*.hh' -o -iname '*.cpp' | xargs clang-format -i
	find tests/ -iname '*.hh' -o -iname '*.cpp' | xargs clang-format -i
