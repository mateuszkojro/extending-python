import unittest
import personal

import numpy as np
import matplotlib.pyplot as plt

# l = personal.fit_line([1,2,3])
# print(l)
# l = personal.full_list(10)
# print(l, len(l))

class TestPersonal(unittest.TestCase):

    def test_full_list(self):
        for sz in range(0,50,5):
            self.assertEqual(personal.full_list(sz), [0 for _ in range(sz)])


def t(x):
    return 12 * x + 1000

if __name__ == '__main__':
    xs = np.linspace(0,100)
    ys = np.array([t(x) for x in xs])

    print(xs)
    print(ys)

    
    a,b,r = personal.fit_line(xs,ys)

    def f(x):
        return a*x +b

    plt.title(f"{a:.2f}x + {b:.2f} and r: {r:.2f}")
    plt.scatter(xs,ys)
    plt.plot(xs, [f(x) for x in xs])
    plt.savefig("result.png")


    # unittest.main()