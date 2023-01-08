import sys
import pygame
from bullet import Bullet
from alien import Alien
from time import sleep


def check_keydown_event(event,ai_settings,screen,ship,bullets):
	# if the event was Keydown, then check the key value.
	if event.key == pygame.K_RIGHT:
		ship.moving_right = True
	elif event.key == pygame.K_LEFT:
		ship.moving_left = True
	elif event.key == pygame.K_SPACE:
		fire_bullet(ai_settings,screen,ship,bullets)
	elif event.key == pygame.K_q:
		sys.exit()


def check_keyup_event(event,ship):
	if event.key == pygame.K_RIGHT:
		ship.moving_right = False
	elif event.key == pygame.K_LEFT:
		ship.moving_left = False


def check_events(ai_settings,screen,stats,sb,play_button,ship,aliens,bullets):
	for event in pygame.event.get():
		# Check the Quit event.
		if event.type == pygame.QUIT: 
			sys.exit()
		# Check the Ketdown event. 
		elif event.type == pygame.KEYDOWN:
			check_keydown_event(event,ai_settings,screen,ship,bullets)
		# Check the Keyup event.
		elif event.type == pygame.KEYUP:
			check_keyup_event(event,ship)
		# Ckeck the mouse button event.
		elif event.type == pygame.MOUSEBUTTONDOWN:
			mouse_x, mouse_y = pygame.mouse.get_pos()
			check_play_button(ai_settings, screen, stats, sb, play_button, ship, aliens, bullets, mouse_x, mouse_y)


def check_play_button(ai_settings, screen, stats, sb, play_button, ship, aliens, bullets, mouse_x, mouse_y):
	button_clicked = play_button.rect.collidepoint(mouse_x, mouse_y)
	if button_clicked and not stats.game_active:
		ai_settings.initialize_dynamic_settings()

		# Make the mouse unvisible
		pygame.mouse.set_visible(False)

		stats.reset_stats()
		stats.game_active = True

		# Reset the scoreboard
		sb.prep_score()
		sb.prep_high_score()
		sb.prep_level()
		sb.prep_ships()

		aliens.empty()
		bullets.empty()

		create_fleet(ai_settings, screen, ship, aliens)
		ship.center_ship()


def update_bullets(ai_settings, screen, stats, sb, ship, aliens, bullets):
	bullets.update()
	# Delete the bullet which can't see.
	for bullet in bullets.copy():
		if bullet.rect.bottom <= 0:
			bullets.remove(bullet)
	# This one can be used to print the number of bullets.
	#print(len(bullets))
	check_bullet_alien_collisions(ai_settings, screen, stats, sb, ship, aliens, bullets)


def check_bullet_alien_collisions(ai_settings, screen, stats, sb, ship, aliens, bullets):
	# Check if the bullets collision with the aliens.
	# The patam [True,True] can controll if the Obj still remian on the screen after collision.
	collisions = pygame.sprite.groupcollide(bullets, aliens, True, True)
	
	if collisions :
		for aliens in collisions.values():
			stats.score += ai_settings.alien_points * len(aliens)
			sb.prep_score()
		check_high_score(stats, sb)
	
	if len(aliens) == 0 :
		# Delete all the bullets.
		bullets.empty()
		ai_settings.increase_speed()
		create_fleet(ai_settings, screen, ship, aliens)
		stats.level += 1
		sb.prep_level()


def fire_bullet(ai_settings,screen,ship,bullets):
	# Limit to create a bullet.
	if len(bullets) < ai_settings.bullets_allowed:
		new_bullet = Bullet(ai_settings,screen,ship)
		bullets.add(new_bullet)


def get_number_aliens_x(ai_settings, alien_width):
	available_space_x = ai_settings.screen_width - 2 * alien_width
	number_aliens_x = int(available_space_x / ( 2 * alien_width))
	return number_aliens_x 


def get_number_rows(ai_settings, ship_height, alien_height):
	available_space_y = (ai_settings.screen_height - 
			( 3 * alien_height) - ship_height)
	number_rows = int(available_space_y / (2 * alien_height))
	return number_rows


# Create a row of aliens.
def create_alien(ai_settings, screen, aliens, alien_number, row_number):
	alien = Alien(ai_settings,screen)
	alien_width = alien.rect.width
	alien.x = alien_width + 2 * alien_width * alien_number
	alien.rect.x = alien.x
	alien.rect.y = alien.rect.height + 2 * alien.rect.height * row_number
	aliens.add(alien)


# Create all the aliens, it include numbers of row.
def create_fleet(ai_settings, screen, ship, aliens):
	alien = Alien(ai_settings, screen)
	number_aliens_x = get_number_aliens_x(ai_settings, alien.rect.width)
	number_rows = get_number_rows(ai_settings, ship.rect.height, alien.rect.height)

	for row_number in range(number_rows):
		for alien_number in range(number_aliens_x):
			create_alien(ai_settings, screen, aliens, alien_number, row_number)


def update_aliens(ai_settings, stats, screen, sb, ship, aliens, bullets):
	# This is importan that call the sprite function, the sprite will call the function of each menber. 
	check_fleet_edges(ai_settings, aliens)
	aliens.update()

	# This function accept two param, the first one was a sprite, and the next was a sprite group.
	if pygame.sprite.spritecollideany(ship, aliens):
		ship_hit(ai_settings, stats, screen, sb, ship, aliens, bullets)

	check_aliens_bottom(ai_settings, stats, screen, sb, ship, aliens, bullets)


def check_fleet_edges(ai_settings, aliens):
	for alien in aliens.sprites():
		if alien.check_edges():
			change_fleet_direction(ai_settings, aliens)
			break


def change_fleet_direction(ai_settings,aliens):
	for alien in aliens.sprites() :
		alien.rect.y += ai_settings.fleet_drop_speed
	ai_settings.fleet_direction *= -1


def update_screen(settings,screen,stats,sb,ship,aliens,bullets,play_button):
	screen.fill(settings.bg_color)
	
	for bullet in bullets.sprites():
		bullet.draw_bullet()
	# Draw the ship on the background.
	ship.blitme()
	aliens.draw(screen)
	sb.show_score()
	
	if not stats.game_active:
		play_button.draw_button()
	# Make the last screen dispaly.
	pygame.display.flip()


def ship_hit(ai_settings, stats, screen, sb, ship, aliens, bullets):
	if stats.ships_left > 1:
		stats.ships_left -= 1
	
		sb.prep_ships()
		
		aliens.empty()
		bullets.empty()

		create_fleet(ai_settings, screen, ship, aliens)
		ship.center_ship()

		sleep(0.5)
	else :
		stats.game_active = False
		# Make the moust visible.
		pygame.mouse.set_visible(True)


def check_aliens_bottom(ai_settings, stats, screen, sb, ship, aliens, bullets):
	screen_rect = screen.get_rect()
	for alien in aliens.sprites():
		if alien.rect.bottom >= screen_rect.bottom:
			ship_hit(ai_settings, stats, screen, sb, ship, aliens, bullets)
			break


def check_high_score(stats, sb):
	if stats.score > stats.high_score:
		stats.high_score = stats.score
		sb.prep_high_score()
