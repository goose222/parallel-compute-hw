TEST_SOURCE = rbf.cu

TARGETBIN := ./rbf

CC = nvcc

$(TARGETBIN):$(TEST_SOURCE)
	$(CC)  $(TEST_SOURCE) -o $(TARGETBIN)

.PHONY:clean
clean:
	-rm -rf $(TARGETBIN)