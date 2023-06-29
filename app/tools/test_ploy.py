import matplotlib.pyplot as plt
import numpy as np

import mpl_interactions.ipyplot as iplt

x = np.linspace(0, np.pi, 100)
tau = np.linspace(1, 10, 100)
beta = np.linspace(0.001, 1)


def f(x, tau, beta):
    return np.sin(x * tau) * x**beta


fig, ax = plt.subplots()
controls = iplt.plot(x, f, tau=tau, beta=beta, slider_formats={"beta": "{:.3e}"})
plt.show()