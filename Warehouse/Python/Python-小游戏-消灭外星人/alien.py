import pygame
from pygame.sprite import Sprite

class Alien(Sprite):
	def __init__(self,ai_settings,screen):
		super(Alien,self).__init__()
		self.screen = screen
		self.ai_settings = ai_settings
		
		# Load the monster image.
		self.image = pygame.image.load('images/monster-small.png')
		# Getting the monster rectangle.
		self.rect = self.image.get_rect()
		
		self.rect.x = self.rect.width*2
		self.rect.y = self.rect.height*2
		
		
		self.x = float(self.rect.x)
		
	def blitme(self):
		self.screen.blit(self.image,self.rect)

	def update(self):
		# Move to the right.
		self.x += ( self.ai_settings.alien_speed_factor * 
					self.ai_settings.fleet_direction)
		self.rect.x = self.x

	def check_edges(self):
		# If the alien reached the margin, return True.
		screen_rect = self.screen.get_rect()
		if self.rect.right >= screen_rect.right:
			return True
		elif self.rect.left <= 0 :
			return True
