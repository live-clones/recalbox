

env:
	python -m venv env
	source env/bin/activate
	pip install pytest pytest-cov pytest-subprocess pytest-mock

coverage: env
	pytest --cov=configgen
