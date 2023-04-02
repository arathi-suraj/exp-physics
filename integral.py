iter = 30
sep = 30
x_max = 66
y_max = 13
A = 1242.0
V = 3117.0
r = 2*V/A

l = A**2/(4*np.pi*V)
x_range = np.linspace(0, x_max, iter)
y_range = np.linspace(0, y_max, iter)
phi_range = np.linspace(0, np.pi, iter)
x1_range = np.linspace(0, l, iter)

def f (x, y, phi, x1):
  denom = (x1 - x)**2 + (r*np.cos(phi) - y_max/2 - y)**2 + (sep - r*np.sin(phi))**2
  arg = (np.sqrt((x1 - x)**2 + (r*np.cos(phi) - y_max/2 - y)**2)) / (sep - r*np.sin(phi))
  numer = np.cos(np.arctan(arg))
  return(numer/denom)


eval = []
for x_ind in range(0, iter):
  for y_ind in range(0, iter):
    for phi_ind in range(0, iter):
      for x1_ind in range(0, iter):
        try:
          area = (x_range[x_ind+1] - x_range[x_ind])*(y_range[y_ind+1] - y_range[y_ind])*(phi_range[phi_ind+1] - phi_range[phi_ind])*(x1_range[x1_ind+1] - x1_range[x1_ind])
          f_eval = f(x_range[x_ind], y_range[y_ind], phi_range[phi_ind], x1_range[x1_ind])
          eval.append(f_eval*area)
        except IndexError:
          pass

ans = 0
for val in eval:
  ans += val

print(ans)
