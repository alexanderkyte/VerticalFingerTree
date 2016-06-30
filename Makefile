dest:=build

ifneq "$(wildcard $(dest) )" ""
$(shell "echo mkdir $(dest)")
endif

ifndef MONO
MONO=mono
endif

.PHONY: run-test-all
run-test-all: $(dest)/SpineTest.exe
	$(MONO) $(dest)/SpineTest.exe

$(dest)/SpineTest.exe: test/SpineTest.cs $(dest)/Spine.dll 
	mcs $(dest)/Spine.dll test/SpineTest.cs -out:$@

$(dest)/Spine.dll: src/Spine.cs
	mcs -target:library src/Spine.cs -out:$@
