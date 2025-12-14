import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

export interface TokenResponse {
  id: number;
  value: string;
  user_id: string;
  created_at: string;
  used: boolean;
}

export interface ResultsResponse {
  A: number;
  B: number;
  C: number;
}

export interface SessionResponse {
  id: number;
  name: string;
  started_at: string;
  active: boolean;
}

@Injectable({
  providedIn: 'root'
})
export class VoteService {
  private baseUrl = 'http://127.0.0.1:8000/api';

  constructor(private http: HttpClient) {}

  getCurrentSession(): Observable<SessionResponse> {
    return this.http.get<SessionResponse>(`${this.baseUrl}/session/current/`);
  }

  startNewSession(name: string): Observable<SessionResponse> {
    return this.http.post<SessionResponse>(`${this.baseUrl}/session/start/`, { name });
  }

  restartSession(name: string): Observable<SessionResponse> {
    return this.http.post<SessionResponse>(`${this.baseUrl}/session/restart/`, { name });
  }

  stopSession(): Observable<SessionResponse> {
    return this.http.post<SessionResponse>(`${this.baseUrl}/session/stop/`, {});
  }

  createToken(userId: string): Observable<TokenResponse> {
    return this.http.post<TokenResponse>(`${this.baseUrl}/tokens/`, {
      user_id: userId
    });
  }

  sendVote(userId: string, tokenValue: string, option: string): Observable<any> {
    return this.http.post(`${this.baseUrl}/votes/`, {
      user_id: userId,
      token_value: tokenValue,
      option: option
    });
  }

  getResults(): Observable<ResultsResponse> {
    return this.http.get<ResultsResponse>(`${this.baseUrl}/results/`);
  }
    resumeSession(): Observable<SessionResponse> {
    return this.http.post<SessionResponse>(`${this.baseUrl}/session/resume/`, {});
  }

}


