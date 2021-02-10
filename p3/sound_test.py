#!/usr/bin/python2
import pygame

pygame.mixer.init()

def play_music(musicfile):
    pygame.mixer.music.load(musicfile)
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy() == True:
        continue


play_music('music/yakety.mp3')

