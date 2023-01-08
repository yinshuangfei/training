class Settings():
	"""Storage the Monster setting."""
	def __init__(self):
		# Setting the screen.
		self.screen_width = 900
		self.screen_height = 600

		# Setting the background color.
		self.bg_color = (230,230,230)

		self.ship_limit = 3

		# Setting the bullet.
		self.bullet_width = 3
		self.bullet_height = 15
		self.bullet_color = 60,60,60
		self.bullets_allowed = 3

		self.fleet_drop_speed = 8

		# The game speed.
		self.speedup_scale = 1.1
		
		self.score_scale = 1.5
		self.alien_points = 50

		self.initialize_dynamic_settings()



	def initialize_dynamic_settings(self):
		# Setting the ship speed.
		self.ship_speed_factor = 3 #0.9
		self.bullet_speed_factor = 5 #1.5
		
		# Setting the alien.
		self.alien_speed_factor = 0.1
		
		# The fleet direcion, 1 means right, and -1 means left.
		self.fleet_direction = 1


	def increase_speed(self):
		# Speed up and make the score up.
		self.ship_speed_factor *= self.speedup_scale
		self.bullet_speed_factor *= self.speedup_scale
		self.alien_speed_factor *= self.speedup_scale

		self.alien_points = int(self.alien_points * self.score_scale)
