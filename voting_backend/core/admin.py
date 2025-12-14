from django.contrib import admin

from .models import Token, Vote

admin.site.register(Token)
admin.site.register(Vote)
