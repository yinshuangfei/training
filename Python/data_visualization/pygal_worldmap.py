#!/bin/env python3
# pip3 install pygal pygal_maps_world


import pygal

worldmap_chart = pygal.maps.world.World()
worldmap_chart.title = 'World'

worldmap_chart.add("高", {
	"cn": 6500,
	"us": 6000,
	"de": 5500,  # 德国
	"jp": 5000,
	"in": 4500,  # 印度
	"ru": 4000,  # 俄国
	"fr": 3500,  # 法国
	"hk": 3000,
	"tw": 2500,
	"it": 2000,  # 意大利
	"kr": 1500,  # 韩国
	"mo": 1000,  # Macao，澳门
	"gb": 500,  # United Kingdom，英国
})

worldmap_chart.add("中", {
	"ca",
	"it",
	"br",
	"th",
	"ar",
	"co",
	"pt",
	"pk",
	"pl",
	"bd",
	"se",
	"tr",
	"ve",
	"gb",
})

worldmap_chart.add("低", {
	"nl",
	"hu",
	"cl",
	"ph",
	"my",
	"kr",
	"uy",
	"ro",
	"eg",
	"ke",
	"kp",
	"cz",
	"sk",
	"mg",
})

worldmap_chart.render_to_file('americas.svg')