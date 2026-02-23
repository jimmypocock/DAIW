.PHONY: build run clean configure test-python ai-service

# C++ Application
configure:
	cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
	cmake --build build

run: build
	./build/DAIW_artefacts/Debug/DAIW.app/Contents/MacOS/DAIW

clean:
	rm -rf build

# Python AI Service
ai-service:
	cd ai-service && source venv/bin/activate && python service.py

ai-setup:
	cd ai-service && python3 -m venv venv && source venv/bin/activate && pip install -r requirements.txt

test-python:
	cd ai-service && source venv/bin/activate && pytest

# Development helpers
xcode:
	cmake -B build-xcode -G Xcode
	open build-xcode/DAIW.xcodeproj

format:
	find src -name '*.cpp' -o -name '*.h' | xargs clang-format -i

# Full rebuild
rebuild: clean build
