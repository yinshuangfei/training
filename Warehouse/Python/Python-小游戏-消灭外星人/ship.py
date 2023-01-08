import pygame
from pygame.sprite import Sprite

class Ship(Sprite):
	def __init__(self,ai_settings,screen):
		super(Ship, self).__init__()
		# get the screen. 
		self.screen = screen
		# The key sensitivity
		self.sensitivity = ai_settings.ship_speed_factor

		# input the image and get it's bounding rectangle.
		self.image = pygame.image.load("images/tractor.png")
		self.rect = self.image.get_rect()
		self.screen_rect = screen.get_rect()

		# put the ship to center of the bottom. The position 
		# can choose from center|centerx|centery.
		# The param bottom can also choose from top|bottom|left|right.
		# In pygame, the pisition (0,0) was letf top, and (max,max)
		# was right bottom.
		self.rect.centerx = self.screen_rect.centerx
		self.rect.bottom = self.screen_rect.bottom

		# Set the value to control the ship keep moving.
		self.moving_left = False
		self.moving_right = False
		
		self.center = float(self.rect.centerx)

	def blitme(self):
		# draw the ship
		self.screen.blit(self.image,self.rect)

	
	def update(self):
		# This check make the ship keep moving while the keydown.
		if self.moving_left and self.rect.left > 0 :
			self.center -=self.sensitivity
		if self.moving_right and self.rect.right < \
								self.screen_rect.right :
			self.center +=self.sensitivity

		self.rect.centerx = self.center
	
	def center_ship(self):
		self.center = self.screen_rect.centerx
