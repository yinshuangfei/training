import pygame
import sys
import random
import time 
from setting import Settings
from ship import Ship
from game_stats import GameStats
from button import Button
from scoreboard import Scoreboard


# Inportant, it is for create a group of Objs.
from pygame.sprite import Group
import game_function as gf

"""
fun random:random.random()
fun sleep:time.sleep(1),the unit is second, it can be type of float.
convert -resize 8 ammo.png ammo-small.png,resize a image.
"""

def start_game():
	print ("game started.")
	pygame.init()	

	ai_settings = Settings()
	screen=pygame.display.set_mode(
		(ai_settings.screen_width,ai_settings.screen_height))
	pygame.display.set_caption("Kill Monster")

	play_button = Button(ai_settings, screen, "Play")

	ship = Ship(ai_settings,screen)
	stats = GameStats(ai_settings)
	sb = Scoreboard(ai_settings, screen, stats)

	# This is not a single bullet.
	bullets = Group() 
	aliens = Group()
	gf.create_fleet(ai_settings, screen, ship, aliens)

	while True:
		gf.check_events(ai_settings,screen,stats,sb,play_button,ship,aliens,bullets)
		
		if stats.game_active:
			ship.update()
			gf.update_bullets(ai_settings, screen, stats, sb, ship, aliens, bullets)
			gf.update_aliens(ai_settings, stats, screen, sb, ship, aliens, bullets)
		
		gf.update_screen(ai_settings,screen,stats,sb,ship,aliens,bullets,play_button)
		
# Started the game
start_game()
