from django.db import models

# Create your models here.

class Session(models.Model):
    name = models.CharField(max_length=64, default='Sesiune implicită')
    started_at = models.DateTimeField(auto_now_add=True)
    active = models.BooleanField(default=True)

    def __str__(self):
        return f'{self.name} ({self.started_at:%Y-%m-%d %H:%M})'


class Token(models.Model):
    session = models.ForeignKey(Session, on_delete=models.CASCADE)
    user_id = models.CharField(max_length=64)
    value = models.CharField(max_length=32, unique=True)
    created_at = models.DateTimeField(auto_now_add=True)
    used = models.BooleanField(default=False)

    class Meta:
        unique_together = ('session', 'user_id')

    def __str__(self):
        return f'{self.user_id}@{self.session_id} -> {self.value}'


class Vote(models.Model):
    OPTION_CHOICES = [
        ('A', 'Option A'),
        ('B', 'Option B'),
        ('C', 'Option C'),
    ]
    session = models.ForeignKey(Session, on_delete=models.CASCADE)
    token = models.OneToOneField(Token, on_delete=models.CASCADE)
    option = models.CharField(max_length=1, choices=OPTION_CHOICES)
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        unique_together = ('session', 'token')

    def __str__(self):
        return f'{self.session_id}: {self.token.user_id} -> {self.option}'