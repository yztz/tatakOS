define get_from_json
$(shell cat $(1) | python3 -c "import sys, json; data=json.load(sys.stdin); print(data['$(2)'])")
endef