import { Component, OnInit, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { interval, Subscription } from 'rxjs';
import { VoteService, ResultsResponse, SessionResponse } from './services/vote.service';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [CommonModule, FormsModule],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent implements OnInit, OnDestroy {
  title = 'STM32 Voting Demo';

  currentToken: string | null = null;
  selectedOption = 'A';
  results: ResultsResponse = { A: 0, B: 0, C: 0 };
  statusMessage = '';
  userId = '';
  session: SessionResponse | null = null;

  sessionUptimeSeconds = 0;
  private timerSub: Subscription | null = null;

  constructor(private voteService: VoteService) {}

  ngOnInit(): void {
    this.loadSession();
    this.loadResults();
  }

  ngOnDestroy(): void {
    if (this.timerSub) {
      this.timerSub.unsubscribe();
    }
  }

  private startSessionTimer(): void {
    if (!this.session) {
      this.sessionUptimeSeconds = 0;
      if (this.timerSub) {
        this.timerSub.unsubscribe();
        this.timerSub = null;
      }
      return;
    }
    const startedAt = new Date(this.session.started_at).getTime();
    if (this.timerSub) {
      this.timerSub.unsubscribe();
    }
    this.timerSub = interval(1000).subscribe(() => {
      const now = Date.now();
      this.sessionUptimeSeconds = Math.floor((now - startedAt) / 1000);
    });
  }

  loadSession(): void {
    this.voteService.getCurrentSession().subscribe({
      next: (res) => {
        this.session = res;
        this.startSessionTimer();
      },
      error: () => {
        this.session = null;
        this.startSessionTimer();
      }
    });
  }

  startSession(): void {
    const name = prompt('Numele sesiunii de vot:', '');
    if (!name || !name.trim()) {
      this.statusMessage = 'Introdu un nume pentru sesiune.';
      return;
    }
    this.voteService.startNewSession(name.trim()).subscribe({
      next: (res) => {
        this.session = res;
        this.results = { A: 0, B: 0, C: 0 };
        this.currentToken = null;
        this.statusMessage = 'Sesiune nouă pornită';
        this.startSessionTimer();
      },
      error: () => {
        this.statusMessage = 'Eroare la pornirea sesiunii';
      }
    });
  }

  restartSession(): void {
    const name = prompt('Numele noii sesiuni (restart):', '');
    if (!name || !name.trim()) {
      this.statusMessage = 'Introdu un nume pentru sesiune.';
      return;
    }
    if (!confirm('Ești sigur că vrei să ștergi toate voturile și tokenurile?')) {
      return;
    }
    this.voteService.restartSession(name.trim()).subscribe({
      next: (res) => {
        this.session = res;
        this.results = { A: 0, B: 0, C: 0 };
        this.currentToken = null;
        this.statusMessage = 'Sesiune restartată (date vechi șterse)';
        this.startSessionTimer();
      },
      error: () => {
        this.statusMessage = 'Eroare la restartul sesiunii';
      }
    });
  }

  stopSession(): void {
    if (!this.session) {
      this.statusMessage = 'Nu există sesiune activă de oprit.';
      return;
    }
    if (!confirm('Oprirea sesiunii va bloca voturile noi, dar păstrează datele. Continui?')) {
      return;
    }
    this.voteService.stopSession().subscribe({
      next: (res) => {
        this.session = { ...res, active: false };
        this.statusMessage = 'Sesiune oprită. Nu se mai acceptă voturi noi.';
        this.startSessionTimer();
      },
      error: () => {
        this.statusMessage = 'Eroare la oprirea sesiunii';
      }
    });
  }

  generateToken(): void {
    if (!this.session || !this.session.active) {
      this.statusMessage = 'Începe și pornește mai întâi o sesiune de vot.';
      return;
    }
    if (!this.userId.trim()) {
      this.statusMessage = 'Introdu un ID de utilizator înainte de generarea tokenului';
      return;
    }
    this.voteService.createToken(this.userId.trim()).subscribe({
      next: (res) => {
        this.currentToken = res.value;
        this.statusMessage = 'Token generat pentru utilizatorul curent';
      },
      error: (err) => {
        if (err.error && typeof err.error === 'object') {
          this.statusMessage = 'Eroare: ' + JSON.stringify(err.error);
        } else {
          this.statusMessage = 'Eroare la generarea tokenului';
        }
      }
    });
  }

  castVote(): void {
    if (!this.session || !this.session.active) {
      this.statusMessage = 'Nu există sesiune activă. Pornește una nouă.';
      return;
    }
    if (!this.userId.trim()) {
      this.statusMessage = 'Introdu un ID de utilizator';
      return;
    }
    if (!this.currentToken) {
      this.statusMessage = 'Generează sau introdu tokenul înainte de a vota';
      return;
    }
    this.voteService.sendVote(this.userId.trim(), this.currentToken, this.selectedOption).subscribe({
      next: (res) => {
        if (res.status === 'OK') {
          this.statusMessage = 'Vot înregistrat pentru această sesiune';
          this.currentToken = null;
          this.loadResults();
        } else {
          this.statusMessage = 'Vot respins';
        }
      },
      error: (err) => {
        if (err.error && err.error.errors) {
          this.statusMessage = 'Eroare: ' + JSON.stringify(err.error.errors);
        } else {
          this.statusMessage = 'Eroare la trimiterea votului';
        }
      }
    });
  }

    resumeSessionClient(): void {
    this.voteService.resumeSession().subscribe({
      next: (res) => {
        this.session = res;
        this.statusMessage = 'Sesiune reluată';
        this.startSessionTimer();
      },
      error: (err) => {
        this.statusMessage = err.error?.detail || 'Nu există sesiune de reluat';
      }
    });
  }


  loadResults(): void {
    this.voteService.getResults().subscribe({
      next: (res) => (this.results = res),
      error: () => {}
    });
  }

  get sessionUptimeLabel(): string {
    const s = this.sessionUptimeSeconds;
    const h = Math.floor(s / 3600);
    const m = Math.floor((s % 3600) / 60);
    const sec = s % 60;
    const parts: string[] = [];
    if (h > 0) parts.push(`${h}h`);
    if (m > 0 || h > 0) parts.push(`${m}m`);
    parts.push(`${sec}s`);
    return parts.join(' ');
  }
}
