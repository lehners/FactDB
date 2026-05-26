# ---- Config ----
PYTHON := python3

RAW_DIR := bench/results/raw
PARSED_DIR := bench/results/parsed
CSV_DIR := bench/results/csv
TEX_DIR := bench/tex

# Discover inputs
RAW_FILES := $(wildcard $(RAW_DIR)/*)

# Stamp files to represent completed stages
PARSED_STAMP := $(PARSED_DIR)/.done

# Map TeX → plotting script
$(TEX_DIR)/runtimes_benchmarks.tex: PLOT_SCRIPT=bench/py/plot/ce.py

# ---- Default ----
.PHONY: all
all: $(TEX_DIR)/build/ablation.pdf $(TEX_DIR)/build/runtimes_micro.pdf $(TEX_DIR)/build/runtimes_benchmarks.pdf

# ---- Step 1: Parse raw → parsed ----
$(PARSED_STAMP): $(RAW_FILES) bench/py/parse_raws.py
	@echo "### Parsing raw benchmark results... ###"
	$(PYTHON) -m bench.py.parse_raws
	@touch $@

# ---- Step 2: parsed → csv (via plotting scripts) ----
$(CSV_DIR)/micro/.done: $(PARSED_STAMP) bench/py/plot/micro.py
	@echo "### Generating CSV data for alphaZipf... ###"
	$(PYTHON) -m bench.py.plot.micro
	@touch $@

$(CSV_DIR)/ce/runtimes/.done: $(PARSED_STAMP) bench/py/plot/runtime_distributions.py
	@echo "### Generating CSV data for alphaZipf... ###"
	$(PYTHON) -m bench.py.plot.runtime_distributions ce
	@mkdir -p $(CSV_DIR)/ce/runtimes
	@touch $@

$(CSV_DIR)/ssb/sf1/runtimes/.done: $(PARSED_STAMP) bench/py/plot/runtime_distributions.py
	@echo "### Generating CSV data for alphaZipf... ###"
	$(PYTHON) -m bench.py.plot.runtime_distributions ssb_sf1
	@mkdir -p $(CSV_DIR)/ssb/sf1/runtimes
	@touch $@

$(CSV_DIR)/ssb/sf10/runtimes/.done: $(PARSED_STAMP) bench/py/plot/runtime_distributions.py
	@echo "### Generating CSV data for alphaZipf... ###"
	$(PYTHON) -m bench.py.plot.runtime_distributions ssb_sf10
	@mkdir -p $(CSV_DIR)/ssb/sf10/runtimes
	@touch $@

$(CSV_DIR)/ce/ablation/.done: $(PARSED_STAMP) bench/py/plot/ablation.py
	@echo "### Generating CSV data for alphaZipf... ###"
	$(PYTHON) -m bench.py.plot.ablation --no-plot
	@mkdir -p $(CSV_DIR)/ce/ablation
	@touch $@

# ---- Step 3: csv → pdf via tex plot ----
$(TEX_DIR)/build/ablation.pdf: $(CSV_DIR)/ce/ablation/.done bench/tex/ablation.tex
	@mkdir -p $(TEX_DIR)/build
	@echo "Ensuring pdf target $@ is up to date"
	@pdflatex -output-directory=bench/tex/build bench/tex/ablation.tex

$(TEX_DIR)/build/runtimes_benchmarks.pdf: $(CSV_DIR)/ce/runtimes/.done $(CSV_DIR)/ssb/sf1/runtimes/.done $(CSV_DIR)/ssb/sf10/runtimes/.done bench/tex/runtimes_benchmarks.tex
	@mkdir -p $(TEX_DIR)/build
	@echo "Ensuring pdf target $@ is up to date"
	@pdflatex -output-directory=bench/tex/build bench/tex/runtimes_benchmarks.tex

$(TEX_DIR)/build/runtimes_micro.pdf: $(CSV_DIR)/micro/.done bench/tex/runtimes_micro.tex
	@mkdir -p $(TEX_DIR)/build
	@echo "Ensuring pdf target $@ is up to date"
	@pdflatex -output-directory=bench/tex/build bench/tex/runtimes_micro.tex


# ---- Cleanup ----
.PHONY: clean
clean:
	rm -rf $(PARSED_DIR) $(CSV_DIR) $(TEX_DIR)/build
