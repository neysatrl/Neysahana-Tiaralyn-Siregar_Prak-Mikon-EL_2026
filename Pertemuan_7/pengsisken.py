import numpy as np

# 1. Definisi fungsi matriks rotasi 2D
def rot2d(deg):
    th = np.radians(deg)  # Konversi sudut derajat ke radian
    c, s = np.cos(th), np.sin(th)  # Hitung nilai cosinus dan sinus
    return np.array([[c, -s], [s, c]])  # Matriks rotasi 2D

# 2. Definisi fungsi matriks rotasi elementer 3D
def rot_x(deg):
    th = np.radians(deg)  # Konversi sudut derajat ke radian
    c, s = np.cos(th), np.sin(th)  # Hitung nilai cosinus dan sinus
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])  # Matriks rotasi 3D sumbu-x (Roll)

def rot_y(deg):
    th = np.radians(deg)  # Konversi sudut derajat ke radian
    c, s = np.cos(th), np.sin(th)  # Hitung nilai cosinus dan sinus
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])  # Matriks rotasi 3D sumbu-y (Pitch)

def rot_z(deg):
    th = np.radians(deg)  # Konversi sudut derajat ke radian
    c, s = np.cos(th), np.sin(th)  # Hitung nilai cosinus dan sinus
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])  # Matriks rotasi 3D sumbu-z (Yaw)

# --- ROTASI 2D ---

# Kasus 1
P_awal = np.array([20.0, 30.0])  # Titik koordinat awal [x, y]
R = rot2d(15.0)  # Hitung matriks rotasi untuk theta = 15 derajat
P_akhir = R @ P_awal  # Transformasi maju: P_akhir = R @ P_awal
print("2D Kasus 1 P_akhir:", P_akhir)

# Kasus 2
P_akhir = np.array([10.0, 30.0])  # Titik koordinat akhir [x, y]
R = rot2d(35.0)  # Hitung matriks rotasi untuk theta = 35 derajat
P_awal = R.T @ P_akhir  # Transformasi balik: P_awal = R.T @ P_akhir
print("2D Kasus 2 P_awal :", P_awal)

# Kasus 3
P_awal = np.array([1.0, 30.0])  # Titik koordinat awal [x, y]
R = rot2d(-45.0)  # Hitung matriks rotasi untuk theta = -45 derajat
P_akhir = R @ P_awal  # Transformasi maju: P_akhir = R @ P_awal
print("2D Kasus 3 P_akhir:", P_akhir)

# Kasus 4
P_akhir = np.array([1.0, 10.0])  # Titik koordinat akhir [x, y]
R = rot2d(30.0)  # Hitung matriks rotasi untuk theta = 30 derajat
P_awal = R.T @ P_akhir  # Transformasi balik: P_awal = R.T @ P_akhir
print("2D Kasus 4 P_awal :", P_awal)


# --- ROTASI 3D ---

# Kasus 1
P_awal = np.array([20.0, 30.0, 60.0])  # Titik koordinat awal [x, y, z]
R_total = rot_y(45.0) @ rot_z(20.0) @ rot_x(15.0)  # Komposisi rotasi (urutan: Roll -> Yaw -> Pitch)
P_akhir = R_total @ P_awal  # Transformasi maju: P_akhir = R_total @ P_awal
print("3D Kasus 1 P_akhir:", P_akhir)

# Kasus 2
P_awal = np.array([10.0, 30.0, 20.0])  # Titik koordinat awal [x, y, z]
R_total = rot_x(45.0) @ rot_z(20.0) @ rot_y(25.0)  # Komposisi rotasi (urutan: Pitch -> Yaw -> Roll)
P_akhir = R_total @ P_awal  # Transformasi maju: P_akhir = R_total @ P_awal
print("3D Kasus 2 P_akhir:", P_akhir)